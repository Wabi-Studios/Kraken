//
// Copyright 2020 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//

#include "wabi/imaging/hdPh/textureHandleRegistry.h"

#include "wabi/imaging/hdPh/samplerObjectRegistry.h"
#include "wabi/imaging/hdPh/textureHandle.h"
#include "wabi/imaging/hdPh/textureObject.h"
#include "wabi/imaging/hdPh/textureObjectRegistry.h"

#include <mutex>
#include <unordered_map>

WABI_NAMESPACE_BEGIN

class HdPh_TextureHandleRegistry::_TextureToHandlesMap
{
 public:
  using TextureSharedPtr = HdPhTextureObjectSharedPtr;

  using HandlePtr = HdPhTextureHandlePtr;
  using HandlePtrVector = tbb::concurrent_vector<HandlePtr>;
  using HandlePtrVectorSharedPtr = std::shared_ptr<HandlePtrVector>;
  using Map = std::unordered_map<TextureSharedPtr, HandlePtrVectorSharedPtr>;

  // Get handles associated to a texture.
  HandlePtrVectorSharedPtr GetHandles(TextureSharedPtr const &texture)
  {
    std::unique_lock<std::mutex> lock(_mutex);

    const typename Map::const_iterator it = _map.find(texture);
    if (it == _map.end())
    {
      return nullptr;
    }

    return it->second;
  }

  // Insert an association of a handle with a texture
  void Insert(TextureSharedPtr const &texture, HandlePtr const &handle)
  {
    _size++;
    _GetOrCreate(texture)->push_back(handle);
  }

  // Remove weak pointers to expired handles that were associated
  // with the given textures.
  // Return true if such texture has no more associated handles (and
  // remove entry for that texture).
  bool GarbageCollect(const std::set<HdPhTextureObjectSharedPtr> &textures)
  {
    bool result = false;

    for (HdPhTextureObjectSharedPtr const &texture : textures)
    {
      const typename Map::const_iterator it = _map.find(texture);
      if (it == _map.end())
      {
        result = true;
      }
      else
      {
        if (_GarbageCollect(it->second.get()))
        {
          result = true;
          _map.erase(it);
        }
      }
    }

    return result;
  }

  const Map &GetMap() const
  {
    return _map;
  }

  size_t GetSize() const
  {
    return _size.load();
  }

 private:
  // Remove all expired weak pointers from vector, return true
  // if no weak pointers left.
  //
  // Also update _size.
  bool _GarbageCollect(HandlePtrVector *const vec)
  {
    // Go from left to right, filling slots that became empty
    // with valid weak pointers from the right.
    size_t last = vec->size();

    for (size_t i = 0; i < last; i++)
    {
      if ((*vec)[i].expired())
      {
        _size--;
        while (true)
        {
          last--;
          if (i == last)
          {
            break;
          }
          if (!(*vec)[last].expired())
          {
            (*vec)[i] = (*vec)[last];
            break;
          }
        }
      }
    }

    vec->resize(last);

    return last == 0;
  }

  // Get the vector of associated handles for given texture
  // or insert empty vector if not accounted for yet.
  HandlePtrVectorSharedPtr _GetOrCreate(TextureSharedPtr const &texture)
  {
    std::unique_lock<std::mutex> lock(_mutex);

    HandlePtrVectorSharedPtr &result = _map[texture];

    if (!result)
    {
      result = std::make_shared<HandlePtrVector>();
    }

    return result;
  }

  std::atomic<size_t> _size;

  std::mutex _mutex;
  Map _map;
};

HdPh_TextureHandleRegistry::HdPh_TextureHandleRegistry(HdPhResourceRegistry *registry)
  : _textureTypeToMemoryRequestChanged(false),
    _samplerObjectRegistry(std::make_unique<HdPh_SamplerObjectRegistry>(registry)),
    _textureObjectRegistry(std::make_unique<HdPh_TextureObjectRegistry>(registry)),
    _textureToHandlesMap(std::make_unique<_TextureToHandlesMap>())
{}

HdPh_TextureHandleRegistry::~HdPh_TextureHandleRegistry() = default;

HdPhTextureHandleSharedPtr HdPh_TextureHandleRegistry::AllocateTextureHandle(
  HdPhTextureIdentifier const &textureId,
  HdTextureType textureType,
  HdSamplerParameters const &samplerParams,
  size_t memoryRequest,
  bool createBindlessHandle,
  HdPhShaderCodePtr const &shaderCode)
{
  TRACE_FUNCTION();

  // Allocate texture (CPU only)
  HdPhTextureObjectSharedPtr const textureObject = _textureObjectRegistry->AllocateTextureObject(
    textureId, textureType);

  HdPhTextureHandleSharedPtr const result = std::make_shared<HdPhTextureHandle>(
    textureObject, samplerParams, memoryRequest, createBindlessHandle, shaderCode, this);

  // Keep track and mark dirty
  _textureToHandlesMap->Insert(textureObject, result);
  _dirtyHandles.push_back(result);
  _dirtyTextures.push_back(textureObject);

  return result;
}

void HdPh_TextureHandleRegistry::MarkSamplerGarbageCollectionNeeded()
{
  _samplerObjectRegistry->MarkGarbageCollectionNeeded();
}

void HdPh_TextureHandleRegistry::MarkDirty(HdPhTextureObjectPtr const &texture)
{
  _dirtyTextures.push_back(texture);
}
void HdPh_TextureHandleRegistry::MarkDirty(HdPhShaderCodePtr const &shader)
{
  _dirtyShaders.push_back(shader);
}

// Compute target memory for texture.
void HdPh_TextureHandleRegistry::_ComputeMemoryRequest(HdPhTextureObjectSharedPtr const &texture)
{
  _TextureToHandlesMap::HandlePtrVectorSharedPtr const handles = _textureToHandlesMap->GetHandles(texture);

  if (!handles)
  {
    return;
  }

  bool hasHandle = false;
  size_t maxRequest = 0;

  // Take maximum of memory requests from all associated
  // handles.
  for (HdPhTextureHandlePtr const &handlePtr : *handles)
  {
    if (HdPhTextureHandleSharedPtr const handle = handlePtr.lock())
    {
      maxRequest = std::max(maxRequest, handle->GetMemoryRequest());
      hasHandle = true;
    }
  }

  if (maxRequest == 0)
  {
    // If no handle had an opinion, use default memory request.
    const auto it = _textureTypeToMemoryRequest.find(texture->GetTextureType());
    if (it != _textureTypeToMemoryRequest.end())
    {
      maxRequest = it->second;
    }
  }

  if (hasHandle)
  {
    texture->SetTargetMemory(maxRequest);
  }
}

// Compute target memory for textures.
void HdPh_TextureHandleRegistry::_ComputeMemoryRequests(const std::set<HdPhTextureObjectSharedPtr> &textures)
{
  TRACE_FUNCTION();

  for (HdPhTextureObjectSharedPtr const &texture : textures)
  {
    _ComputeMemoryRequest(texture);
  }
}

void HdPh_TextureHandleRegistry::_ComputeAllMemoryRequests()
{
  TRACE_FUNCTION();

  for (const auto &it : _textureToHandlesMap->GetMap())
  {
    _ComputeMemoryRequest(it.first);
  }
}

// Turn a vector into a set, dropping expired weak points.
template<typename T>
static void _Uniquify(const tbb::concurrent_vector<std::weak_ptr<T>> &objects,
                      std::set<std::shared_ptr<T>> *result)
{
  TRACE_FUNCTION();

  for (std::weak_ptr<T> const &objectPtr : objects)
  {
    if (std::shared_ptr<T> const object = objectPtr.lock())
    {
      result->insert(object);
    }
  }
}

bool HdPh_TextureHandleRegistry::_GarbageCollectHandlesAndComputeTargetMemory()
{
  TRACE_FUNCTION();

  // Note that dirtyTextures is holding on to shared ptr.
  //
  // This is ok within the context of garbage collecting handles,
  // but not in the context of garbage collecting textures. Garbage
  // collecting textures is happening in a different functions so
  // the dirtyTextures map will be destroyed by then.

  // Creating a std::set might be expensive. Similar comments as for
  // the HdPhTextureObjectRegistry apply (see _Uniquify in
  // textureObjectRegistry.cpp).
  std::set<HdPhTextureObjectSharedPtr> dirtyTextures;
  _Uniquify(_dirtyTextures, &dirtyTextures);

  // Clean-up the texture to handle map
  const bool texturesNeedGarbageCollection = _textureToHandlesMap->GarbageCollect(dirtyTextures);

  // Compute target memory for dirty textures.
  if (_textureTypeToMemoryRequestChanged)
  {
    _ComputeAllMemoryRequests();
    _textureTypeToMemoryRequestChanged = false;
  }
  else
  {
    _ComputeMemoryRequests(dirtyTextures);
  }

  _dirtyTextures.clear();

  return texturesNeedGarbageCollection;
}

void HdPh_TextureHandleRegistry::_GarbageCollectAndComputeTargetMemory()
{
  TRACE_FUNCTION();

  // Garbage collect unused texture handles for the dirty textures,
  // then re-compute their target memory using the remaining texture
  // handles.
  //
  // Note that this also deletes the texture entries in the
  // _textureToHandlesMap if there are no handles left so that the
  // textures can be freed.
  if (!_GarbageCollectHandlesAndComputeTargetMemory())
  {
    return;
  }

  // Garbage collect textures.
  _textureObjectRegistry->GarbageCollect();
}

std::set<HdPhShaderCodeSharedPtr> HdPh_TextureHandleRegistry::_Commit()
{
  TRACE_FUNCTION();

  // Commit textures first. The result is a list of
  // textures that have been committed or recommitted (e.g.,
  // target memory changed or file on disk).
  const std::set<HdPhTextureObjectSharedPtr> committedTextures = _textureObjectRegistry->Commit();

  // Compute the set of new texture handles that have never been
  // processed before and ...
  std::set<HdPhTextureHandleSharedPtr> dirtyHandles;
  _Uniquify(_dirtyHandles, &dirtyHandles);

  // ... texture handles associated to textures affected by the
  // (re)-commit.
  for (HdPhTextureObjectSharedPtr const &texture : committedTextures)
  {
    if (_TextureToHandlesMap::HandlePtrVectorSharedPtr const handles = _textureToHandlesMap->GetHandles(
          texture))
    {
      _Uniquify(*handles, &dirtyHandles);
    }
  }

  // Collect associated shader code instances (who might want to
  // add, e.g., bindless handless or other texture metadata to the
  // shader bar)...
  //
  // Might be replaced by, e.g., std::unordered_set for performance.
  std::set<HdPhShaderCodeSharedPtr> result;

  {
    TRACE_FUNCTION_SCOPE("Determining shader code instances that require updating.");

    for (HdPhTextureHandleSharedPtr const &handle : dirtyHandles)
    {

      // ... while allocating samplers for the textures.
      handle->ReallocateSamplerIfNecessary();

      if (HdPhShaderCodeSharedPtr const shader = handle->GetShaderCode().lock())
      {
        result.insert(shader);
      }
    }

    for (HdPhShaderCodePtr const &dirtyShader : _dirtyShaders)
    {
      if (HdPhShaderCodeSharedPtr const shader = dirtyShader.lock())
      {
        result.insert(shader);
      }
    }
  }

  _dirtyHandles.clear();
  _dirtyShaders.clear();

  // Resource registry uses result to give shader code a chance to
  // add buffer sources.
  return result;
}

std::set<HdPhShaderCodeSharedPtr> HdPh_TextureHandleRegistry::Commit()
{
  // Garbage collect texture handles and textures and compute
  // target memory for the textures before calling the commit
  // on the texture registry.
  _GarbageCollectAndComputeTargetMemory();

  // Commit textures, update samplers associated with the
  // corresponding texture handles. Return all affected shaders.
  const std::set<HdPhShaderCodeSharedPtr> result = _Commit();

  // Updating the samplers in the above _Commit() could have
  // freed some, so we do sampler garbage collection last.
  _samplerObjectRegistry->GarbageCollect();

  return result;
}

void HdPh_TextureHandleRegistry::SetMemoryRequestForTextureType(const HdTextureType textureType,
                                                                const size_t memoryRequest)
{
  size_t &val = _textureTypeToMemoryRequest[textureType];
  if (val != memoryRequest)
  {
    val = memoryRequest;
    _textureTypeToMemoryRequestChanged = true;
  }
}

size_t HdPh_TextureHandleRegistry::GetNumberOfTextureHandles() const
{
  return _textureToHandlesMap->GetSize();
}

WABI_NAMESPACE_END
