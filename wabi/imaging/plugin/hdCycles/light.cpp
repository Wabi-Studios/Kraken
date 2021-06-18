//  Copyright 2020 Tangent Animation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
//  including without limitation, as related to merchantability and fitness
//  for a particular purpose.
//
//  In no event shall any copyright holder be liable for any damages of any kind
//  arising from the use of this software, whether in contract, tort or otherwise.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "light.h"

#include "renderParam.h"
#include "utils.h"

#include "tokens.h"

#include <render/object.h>
#include <render/scene.h>
#include <render/shader.h>
#include <util/util_hash.h>
#include <util/util_math_float3.h>
#include <util/util_string.h>
#include <util/util_transform.h>

#include <wabi/usd/sdf/assetPath.h>
#include <wabi/usd/usdLux/tokens.h>

WABI_NAMESPACE_BEGIN

HdCyclesLight::HdCyclesLight(SdfPath const &id,
                             TfToken const &lightType,
                             HdCyclesRenderDelegate *a_renderDelegate)
  : HdLight(id),
    m_hdLightType(lightType),
    m_cyclesLight(nullptr),
    m_shaderGraphBits(ShaderGraphBits::Default),
    m_renderDelegate(a_renderDelegate)
{
  // Added to prevent fallback lights
  // TODO: Is this the best solution...
  if (id == SdfPath::EmptyPath())
    return;

  _CreateCyclesLight(id, m_renderDelegate->GetCyclesRenderParam());
}

HdCyclesLight::~HdCyclesLight()
{
  if (m_hdLightType == HdPrimTypeTokens->domeLight)
  {
    m_renderDelegate->GetCyclesRenderParam()->Interrupt();
  }

  if (m_cyclesLight)
  {
    if (m_cyclesLight->shader)
    {
      m_renderDelegate->GetCyclesRenderParam()->RemoveShaderSafe(m_cyclesLight->shader);
      delete m_cyclesLight->shader;
    }
    m_renderDelegate->GetCyclesRenderParam()->RemoveLightSafe(m_cyclesLight);
    delete m_cyclesLight;
  }
}

void HdCyclesLight::Finalize(HdRenderParam *renderParam)
{}

void HdCyclesLight::_CreateCyclesLight(SdfPath const &id, HdCyclesRenderParam *renderParam)
{
  ccl::Scene *scene = renderParam->GetCyclesScene();
  m_cyclesLight = new ccl::Light();

  m_cyclesLight->name = ccl::ustring(id.GetName().c_str());

  ccl::Shader *shader = new ccl::Shader();

  m_cyclesLight->shader = shader;

  if (m_hdLightType == HdPrimTypeTokens->domeLight)
  {
    m_cyclesLight->type = ccl::LIGHT_BACKGROUND;
    shader->set_graph(_GetDefaultShaderGraph(true));
    renderParam->SetBackgroundShader(shader);
  }
  else
  {
    if (m_hdLightType == HdPrimTypeTokens->diskLight)
    {
      m_cyclesLight->type = ccl::LIGHT_AREA;
      m_cyclesLight->round = true;

      m_cyclesLight->size = 1.0f;
    }
    else if (m_hdLightType == HdPrimTypeTokens->sphereLight)
    {
      m_cyclesLight->type = ccl::LIGHT_POINT;
    }
    else if (m_hdLightType == HdPrimTypeTokens->distantLight)
    {
      m_cyclesLight->type = ccl::LIGHT_DISTANT;
    }
    else if (m_hdLightType == HdPrimTypeTokens->rectLight)
    {
      m_cyclesLight->type = ccl::LIGHT_AREA;
      m_cyclesLight->round = false;

      m_cyclesLight->size = 1.0f;
    }

    shader->set_graph(_GetDefaultShaderGraph());
  }

  renderParam->AddLightSafe(m_cyclesLight);

  renderParam->AddShaderSafe(shader);

  // Set defaults
  m_cyclesLight->use_diffuse = true;
  m_cyclesLight->use_glossy = true;
  m_cyclesLight->use_transmission = true;
  m_cyclesLight->use_scatter = true;
  m_cyclesLight->cast_shadow = true;
  m_cyclesLight->use_mis = true;
  m_cyclesLight->is_portal = false;
  m_cyclesLight->max_bounces = 1024;

  m_cyclesLight->random_id = ccl::hash_uint2(ccl::hash_string(m_cyclesLight->name.c_str()), 0);

  shader->tag_update(scene);
  m_cyclesLight->tag_update(scene);
}

void HdCyclesLight::_SetTransform(const ccl::Transform &a_transform)
{
  if (!m_cyclesLight)
    return;

  m_cyclesLight->tfm = a_transform;

  m_cyclesLight->axisu = ccl::transform_get_column(&a_transform, 0);
  m_cyclesLight->axisv = ccl::transform_get_column(&a_transform, 1);
  m_cyclesLight->co = ccl::transform_get_column(&a_transform, 3);
  m_cyclesLight->dir = -ccl::transform_get_column(&a_transform, 2);
}

ccl::ShaderGraph *HdCyclesLight::_GetDefaultShaderGraph(bool isBackground)
{
  ccl::ShaderGraph *graph = new ccl::ShaderGraph();
  if (isBackground)
  {
    ccl::BackgroundNode *backgroundNode = new ccl::BackgroundNode();
    backgroundNode->color = ccl::make_float3(0.0f, 0.0f, 0.0f);

    backgroundNode->strength = 1.0f;
    graph->add(backgroundNode);

    ccl::ShaderNode *out = graph->output();
    graph->connect(backgroundNode->output("Background"), out->input("Surface"));
  }
  else
  {
    ccl::EmissionNode *emissionNode = new ccl::EmissionNode();
    emissionNode->color = ccl::make_float3(1.0f, 1.0f, 1.0f);
    emissionNode->strength = 1.0f;
    graph->add(emissionNode);

    ccl::ShaderNode *out = graph->output();
    graph->connect(emissionNode->output("Emission"), out->input("Surface"));
  }
  return graph;
}

ccl::ShaderNode *HdCyclesLight::_FindShaderNode(const ccl::ShaderGraph *graph,
                                                const ccl::NodeType *type,
                                                const ccl::ustring name)
{
  for (ccl::ShaderNode *node : graph->nodes)
  {
    if ((node->type == type) && (node->name == name))
    {
      return node;
    }
  }
  return nullptr;
}

void HdCyclesLight::Sync(HdSceneDelegate *sceneDelegate, HdRenderParam *renderParam, HdDirtyBits *dirtyBits)
{
  SdfPath id = GetId();

  HdCyclesRenderParam *param = static_cast<HdCyclesRenderParam *>(renderParam);

  ccl::Scene *scene = param->GetCyclesScene();

  bool light_updated = false;

  // Defaults
  m_cyclesLight->use_diffuse = true;
  m_cyclesLight->use_glossy = true;
  m_cyclesLight->use_transmission = true;
  m_cyclesLight->use_scatter = true;
  m_cyclesLight->use_mis = true;
  m_cyclesLight->is_portal = false;
  m_cyclesLight->samples = 1;
  m_cyclesLight->max_bounces = 1024;
  m_cyclesLight->map_resolution = 0;

  // Always rebuild dome lights on transform change, the transform texture co-ordinate gets
  // optimised/folded out and we can't get it back to tweak...
  if (*dirtyBits & HdLight::DirtyParams ||
      (*dirtyBits & HdLight::DirtyTransform && m_cyclesLight->type == ccl::LIGHT_BACKGROUND))
  {
    light_updated = true;
    ccl::ShaderGraph *oldGraph = m_cyclesLight->shader->graph;

    // Check if we need to rebuild the graph
    ShaderGraphBits shaderGraphBits = ShaderGraphBits::Default;

    VtValue enableTemperature = sceneDelegate->GetLightParamValue(id, HdLightTokens->enableColorTemperature);
    if (enableTemperature.IsHolding<bool>())
    {
      shaderGraphBits = enableTemperature.UncheckedGet<bool>() ?
                          static_cast<ShaderGraphBits>(shaderGraphBits | ShaderGraphBits::Temperature) :
                          shaderGraphBits;
    }

    VtValue iesFile = sceneDelegate->GetLightParamValue(id, HdLightTokens->shapingIesFile);
    if (iesFile.IsHolding<SdfAssetPath>())
    {
      shaderGraphBits = static_cast<ShaderGraphBits>(shaderGraphBits | ShaderGraphBits::IES);
    }

    VtValue textureFile = sceneDelegate->GetLightParamValue(id, HdLightTokens->textureFile);
    if (textureFile.IsHolding<SdfAssetPath>())
    {
      SdfAssetPath ap = textureFile.UncheckedGet<SdfAssetPath>();
      std::string filepath = ap.GetResolvedPath();

      if (filepath.length() > 0)
      {
        shaderGraphBits = static_cast<ShaderGraphBits>(shaderGraphBits | ShaderGraphBits::Texture);
      }
    }

    ccl::ShaderGraph *graph = nullptr;
    ccl::ShaderNode *outNode = nullptr;

    // Ideally we would just check if it is different, however some nodes
    // simplify & fold internally so we have to re-create the graph, so if
    // there is any nodes used, re-create...
    if (shaderGraphBits || shaderGraphBits != m_shaderGraphBits)
    {
      graph = _GetDefaultShaderGraph(m_cyclesLight->type == ccl::LIGHT_BACKGROUND);
      outNode = graph->output()->input("Surface")->link->parent;
      m_shaderGraphBits = shaderGraphBits;
    }
    else
    {
      outNode = oldGraph->output()->input("Surface")->link->parent;
    }

    // -- Common params

    // Color
    VtValue lightColor = sceneDelegate->GetLightParamValue(id, HdLightTokens->color);
    if (lightColor.IsHolding<GfVec3f>())
    {
      GfVec3f v = lightColor.UncheckedGet<GfVec3f>();
      m_cyclesLight->strength = ccl::make_float3(v[0], v[1], v[2]);
    }

    // Normalize
    float size = 1.0f;
    bool normalize = false;
    VtValue normalizeVal = sceneDelegate->GetLightParamValue(id, HdLightTokens->normalize);
    if (normalizeVal.IsHolding<bool>())
    {
      normalize = normalizeVal.UncheckedGet<bool>();
    }

    // Exposure
    float exposure = 1.0f;
    float exposureOffset = 0.0f;
    VtValue exposureValue = sceneDelegate->GetLightParamValue(id, HdLightTokens->exposure);
    if (exposureValue.IsHolding<float>())
    {
      exposure = exposureValue.UncheckedGet<float>();
    }

    // Intensity
    float intensity = 1.0;
    VtValue intensityValue = sceneDelegate->GetLightParamValue(id, HdLightTokens->intensity);
    if (intensityValue.IsHolding<float>())
    {
      intensity = intensityValue.UncheckedGet<float>();
    }

    // Light cast shadow
    m_cyclesLight->cast_shadow = _HdCyclesGetLightParam<bool>(
      id, sceneDelegate, HdLightTokens->shadowEnable, true);

    // TODO:
    // These two params have no direct mapping. Kept for future implementation

    // // Diffuse
    // VtValue diffuse
    //     = sceneDelegate->GetLightParamValue(id, HdLightTokens->diffuse);
    // if (diffuse.IsHolding<float>()) {
    //     m_cyclesLight->use_diffuse = (diffuse.UncheckedGet<float>()
    //                                   == 1.0f);
    // }

    // // Specular
    // VtValue specular
    //     = sceneDelegate->GetLightParamValue(id, HdLightTokens->specular);
    // if (specular.IsHolding<float>()) {
    //     m_cyclesLight->use_glossy = (specular.UncheckedGet<float>()
    //                                  == 1.0f);
    // }

    // Enable Temperature
    ccl::BlackbodyNode *blackbodyNode = nullptr;
    if (shaderGraphBits & ShaderGraphBits::Temperature)
    {
      // Get Temperature
      VtValue temperature = sceneDelegate->GetLightParamValue(id, HdLightTokens->colorTemperature);
      if (temperature.IsHolding<float>())
      {
        if (graph)
        {
          blackbodyNode = new ccl::BlackbodyNode();
          graph->add(blackbodyNode);

          graph->connect(blackbodyNode->output("Color"), outNode->input("Color"));
        }
        else
        {
          blackbodyNode = static_cast<ccl::BlackbodyNode *>(
            _FindShaderNode(oldGraph, ccl::BlackbodyNode::node_type));
        }
        assert(blackbodyNode != nullptr);
        blackbodyNode->temperature = temperature.UncheckedGet<float>();
      }
    }

    // Enable IES profile. Angle scale and normalize are not supported currently.
    // TODO: Perhaps usdCycles could store embedded IES into a string? ->ies can
    // be used instead of ->filename, Blender uses it to store IES profiles in
    // .blend files...
    if (shaderGraphBits & ShaderGraphBits::IES)
    {
      SdfAssetPath ap = iesFile.UncheckedGet<SdfAssetPath>();
      std::string iesfilepath = ap.GetResolvedPath();

      ccl::IESLightNode *iesNode = nullptr;
      if (graph)
      {
        ccl::TextureCoordinateNode *iesTransform = new ccl::TextureCoordinateNode();
        iesTransform->use_transform = true;
        iesTransform->ob_tfm = m_cyclesLight->tfm;
        graph->add(iesTransform);

        iesNode = new ccl::IESLightNode();
        graph->add(iesNode);

        graph->connect(iesTransform->output("Normal"), iesNode->input("Vector"));

        graph->connect(iesNode->output("Fac"), outNode->input("Strength"));
      }
      else
      {
        iesNode = static_cast<ccl::IESLightNode *>(_FindShaderNode(oldGraph, ccl::IESLightNode::node_type));
      }
      assert(iesNode != nullptr);
      iesNode->filename = iesfilepath;
    }

    if (m_hdLightType == HdPrimTypeTokens->rectLight)
    {
      m_cyclesLight->axisu = ccl::transform_get_column(&m_cyclesLight->tfm, 0);
      m_cyclesLight->axisv = ccl::transform_get_column(&m_cyclesLight->tfm, 1);

      VtValue width = sceneDelegate->GetLightParamValue(id, HdLightTokens->width);
      if (width.IsHolding<float>())
        m_cyclesLight->sizeu = width.UncheckedGet<float>();

      VtValue height = sceneDelegate->GetLightParamValue(id, HdLightTokens->height);
      if (height.IsHolding<float>())
        m_cyclesLight->sizev = height.UncheckedGet<float>();

      if (!normalize)
      {
        size = m_cyclesLight->sizeu * m_cyclesLight->sizev;
      }

      exposureOffset = 2.0f;

      if (shaderGraphBits & ShaderGraphBits::Texture)
      {
        SdfAssetPath ap = textureFile.UncheckedGet<SdfAssetPath>();
        std::string filepath = ap.GetResolvedPath();

        ccl::ImageTextureNode *textureNode = nullptr;
        if (graph)
        {
          textureNode = new ccl::ImageTextureNode();
          graph->add(textureNode);
          ccl::GeometryNode *geometryNode = new ccl::GeometryNode();
          graph->add(geometryNode);

          graph->connect(geometryNode->output("Parametric"), textureNode->input("Vector"));

          if ((shaderGraphBits & ShaderGraphBits::Temperature) && blackbodyNode)
          {
            ccl::VectorMathNode *vecMathNode = new ccl::VectorMathNode();
            vecMathNode->type = ccl::NODE_VECTOR_MATH_MULTIPLY;
            graph->add(vecMathNode);

            graph->connect(textureNode->output("Color"), vecMathNode->input("Vector1"));

            graph->connect(blackbodyNode->output("Color"), vecMathNode->input("Vector2"));

            graph->disconnect(outNode->input("Color"));
            graph->connect(vecMathNode->output("Vector"), outNode->input("Color"));
          }
          else
          {
            graph->connect(textureNode->output("Color"), outNode->input("Color"));
          }
        }
        else
        {
          textureNode = static_cast<ccl::ImageTextureNode *>(
            _FindShaderNode(oldGraph, ccl::ImageTextureNode::node_type));
        }
        assert(textureNode != nullptr);
        textureNode->filename = filepath;
      }
    }

    if (m_hdLightType == HdPrimTypeTokens->diskLight)
    {
      // TODO:
      // Disk Lights cannot be ovals, but Blender can export oval lights...
      // This will be fixed in the great light transition when the new light API
      // is released

      // VtValue width = sceneDelegate->GetLightParamValue(id, HdLightTokens->width);
      // if(width.IsHolding<float>())
      //     m_cyclesLight->sizeu = width.UncheckedGet<float>();

      // VtValue height = sceneDelegate->GetLightParamValue(id, HdLightTokens->height);
      // if(height.IsHolding<float>())
      //     m_cyclesLight->sizev = height.UncheckedGet<float>();

      m_cyclesLight->axisu = ccl::transform_get_column(&m_cyclesLight->tfm, 0);
      m_cyclesLight->axisv = ccl::transform_get_column(&m_cyclesLight->tfm, 1);

      VtValue radius = sceneDelegate->GetLightParamValue(id, HdLightTokens->radius);
      if (radius.IsHolding<float>())
      {
        m_cyclesLight->sizeu = radius.UncheckedGet<float>() * 2.0f;
        m_cyclesLight->sizev = radius.UncheckedGet<float>() * 2.0f;
      }

      exposureOffset = 2.0f;

      if (!normalize)
      {
        size = (m_cyclesLight->sizeu * 0.5f) * (m_cyclesLight->sizev * 0.5f) * M_PI_F;
      }
    }

    if (m_hdLightType == HdPrimTypeTokens->cylinderLight)
    {
      // TODO: Implement
      // Cycles has no concept of cylinder lights.
    }

    if (m_hdLightType == HdPrimTypeTokens->sphereLight)
    {
      VtValue radius = sceneDelegate->GetLightParamValue(id, HdLightTokens->radius);
      if (radius.IsHolding<float>())
        m_cyclesLight->size = radius.UncheckedGet<float>();

      exposureOffset = 1.0f;

      if (!normalize)
      {
        size = m_cyclesLight->size * m_cyclesLight->size * 4.0f * M_PI_F;
      }

      // Spot shaping
      VtValue shapingConeAngle = sceneDelegate->GetLightParamValue(id, HdLightTokens->shapingConeAngle);
      if (shapingConeAngle.IsHolding<float>())
      {
        m_cyclesLight->spot_angle = shapingConeAngle.UncheckedGet<float>() * (M_PI_F / 180.0f) * 2.0f;
        m_cyclesLight->type = ccl::LIGHT_SPOT;
      }

      VtValue shapingConeSoftness = sceneDelegate->GetLightParamValue(id,
                                                                      HdLightTokens->shapingConeSoftness);
      if (shapingConeSoftness.IsHolding<float>())
      {
        m_cyclesLight->spot_smooth = shapingConeSoftness.UncheckedGet<float>();
        m_cyclesLight->type = ccl::LIGHT_SPOT;
      }

      if (m_cyclesLight->type == ccl::LIGHT_SPOT)
      {
        exposureOffset += 1.0f;
      }
    }

    if (m_hdLightType == HdPrimTypeTokens->distantLight)
    {
      // TODO: Test this
      VtValue angle = sceneDelegate->GetLightParamValue(id, HdLightTokens->angle);
      if (angle.IsHolding<float>())
      {
        m_cyclesLight->angle = angle.UncheckedGet<float>() * (M_PI_F / 180.0f);
      }

      if (!normalize)
      {
        size = m_cyclesLight->angle * m_cyclesLight->angle;
      }
    }

    if (m_hdLightType == HdPrimTypeTokens->domeLight)
    {
      ccl::BackgroundNode *backgroundNode = static_cast<ccl::BackgroundNode *>(outNode);
      ccl::VectorMathNode *strengthNode = nullptr;
      if (shaderGraphBits & ShaderGraphBits::Texture || shaderGraphBits & ShaderGraphBits::Temperature)
      {
        if (graph)
        {
          strengthNode = new ccl::VectorMathNode();
          strengthNode->type = ccl::NODE_VECTOR_MATH_MULTIPLY;
          strengthNode->name = ccl::ustring("strength");
          strengthNode->vector1 = ccl::make_float3(1.0f, 1.0f, 1.0f);
          graph->add(strengthNode);
          if (blackbodyNode)
          {
            graph->disconnect(backgroundNode->input("Color"));
            graph->connect(blackbodyNode->output("Color"), strengthNode->input("Vector1"));
          }
          graph->connect(strengthNode->output("Vector"), backgroundNode->input("Color"));
        }
        else
        {
          strengthNode = static_cast<ccl::VectorMathNode *>(
            _FindShaderNode(oldGraph, ccl::VectorMathNode::node_type, ccl::ustring("strength")));
        }
        assert(strengthNode != nullptr);
        strengthNode->vector2 = m_cyclesLight->strength * intensity * powf(2.0f, exposure);
      }
      backgroundNode->color = m_cyclesLight->strength * intensity * powf(2.0f, exposure);
      backgroundNode->strength = 1.0f;
      m_cyclesLight->strength = ccl::make_float3(1.0f, 1.0f, 1.0f);

      if (shaderGraphBits & ShaderGraphBits::Texture)
      {
        SdfAssetPath ap = textureFile.UncheckedGet<SdfAssetPath>();
        std::string filepath = ap.GetResolvedPath();

        ccl::EnvironmentTextureNode *backgroundTexture = nullptr;
        if (graph)
        {
          // Add environment texture nodes. We set transform here for dome lights.
          ccl::TextureCoordinateNode *backgroundTransform = new ccl::TextureCoordinateNode();
          backgroundTransform->use_transform = true;
          ccl::Transform transform = HdCyclesExtractTransform(sceneDelegate, id);
          // Reset translation
          transform.x[3] = 0.0f;
          transform.y[3] = 0.0f;
          transform.z[3] = 0.0f;
          backgroundTransform->ob_tfm = transform;
          graph->add(backgroundTransform);

          backgroundTexture = new ccl::EnvironmentTextureNode();
          if (param->GetUpAxis() == HdCyclesRenderParam::UpAxis::Y)
          {
            // Change co-ordinate mapping on environment texture to match other Hydra delegates
            backgroundTexture->tex_mapping.y_mapping = ccl::TextureMapping::Z;
            backgroundTexture->tex_mapping.z_mapping = ccl::TextureMapping::Y;
            backgroundTexture->tex_mapping.scale = ccl::make_float3(-1.0f, 1.0f, 1.0f);
            backgroundTexture->tex_mapping.rotation = ccl::make_float3(0.0f, 0.0f, M_PI_F * -0.5f);
          }

          graph->add(backgroundTexture);

          graph->connect(backgroundTransform->output("Object"), backgroundTexture->input("Vector"));

          if ((shaderGraphBits & ShaderGraphBits::Temperature) && blackbodyNode)
          {
            ccl::VectorMathNode *vecMathNode = new ccl::VectorMathNode();
            vecMathNode->type = ccl::NODE_VECTOR_MATH_MULTIPLY;
            graph->add(vecMathNode);

            graph->connect(backgroundTexture->output("Color"), vecMathNode->input("Vector1"));

            graph->connect(blackbodyNode->output("Color"), vecMathNode->input("Vector2"));

            graph->disconnect(strengthNode->input("Vector1"));
            graph->connect(vecMathNode->output("Vector"), strengthNode->input("Vector1"));
          }
          else
          {
            graph->connect(backgroundTexture->output("Color"), strengthNode->input("Vector1"));
          }
        }
        else
        {
          backgroundTexture = static_cast<ccl::EnvironmentTextureNode *>(
            _FindShaderNode(oldGraph, ccl::EnvironmentTextureNode::node_type));
        }
        assert(backgroundTexture != nullptr);
        backgroundTexture->filename = filepath;
      }
    }
    else
    {
      m_cyclesLight->strength *= intensity * powf(2.0f, exposure + exposureOffset) * size;
    }

    if (graph)
    {
      m_cyclesLight->shader->set_graph(graph);
    }
  }

  m_cyclesLight->use_diffuse = _HdCyclesGetLightParam<bool>(
    id, sceneDelegate, HdCyclesTokens->cyclesLightUseDiffuse, m_cyclesLight->use_diffuse);

  m_cyclesLight->use_glossy = _HdCyclesGetLightParam<bool>(
    id, sceneDelegate, HdCyclesTokens->cyclesLightUseGlossy, m_cyclesLight->use_glossy);

  m_cyclesLight->use_transmission = _HdCyclesGetLightParam<bool>(
    id, sceneDelegate, HdCyclesTokens->cyclesLightUseTransmission, m_cyclesLight->use_transmission);

  m_cyclesLight->use_scatter = _HdCyclesGetLightParam<bool>(
    id, sceneDelegate, HdCyclesTokens->cyclesLightUseScatter, m_cyclesLight->use_scatter);

  m_cyclesLight->use_mis = _HdCyclesGetLightParam<bool>(
    id, sceneDelegate, HdCyclesTokens->cyclesLightUseMis, m_cyclesLight->use_mis);

  m_cyclesLight->is_portal = _HdCyclesGetLightParam<bool>(
    id, sceneDelegate, HdCyclesTokens->cyclesLightIsPortal, m_cyclesLight->is_portal);

  m_cyclesLight->samples = _HdCyclesGetLightParam<int>(
    id, sceneDelegate, HdCyclesTokens->cyclesLightSamples, m_cyclesLight->samples);

  if (m_renderDelegate->GetCyclesRenderParam()->IsSquareSamples())
  {
    m_cyclesLight->samples *= m_cyclesLight->samples;
  }

  m_cyclesLight->max_bounces = _HdCyclesGetLightParam<int>(
    id, sceneDelegate, HdCyclesTokens->cyclesLightMaxBounces, m_cyclesLight->max_bounces);

  m_cyclesLight->map_resolution = _HdCyclesGetLightParam<int>(
    id, sceneDelegate, HdCyclesTokens->cyclesLightMapResolution, m_cyclesLight->map_resolution);

  // TODO: Light is_enabled doesn't seem to have any effect
  if (*dirtyBits & HdChangeTracker::DirtyVisibility)
  {
    light_updated = true;
    m_cyclesLight->is_enabled = sceneDelegate->GetVisible(id);
  }

  // Dome light transform is set always on shader re-creation above
  // (the internal transform gets optimised away, so a shader rebuild is required)
  if (*dirtyBits & HdLight::DirtyTransform && m_cyclesLight->type != ccl::LIGHT_BACKGROUND)
  {
    light_updated = true;
    _SetTransform(HdCyclesExtractTransform(sceneDelegate, id));
  }

  if (light_updated)
  {
    m_cyclesLight->shader->tag_update(scene);
    m_cyclesLight->tag_update(scene);

    param->Interrupt();
  }

  *dirtyBits = HdChangeTracker::Clean;
}

HdDirtyBits HdCyclesLight::GetInitialDirtyBitsMask() const
{
  return HdChangeTracker::AllDirty | HdLight::DirtyParams | HdLight::DirtyTransform;
}

bool HdCyclesLight::IsValid() const
{
  return true;
}

WABI_NAMESPACE_END