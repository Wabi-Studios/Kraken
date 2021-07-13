#
#  Copyright 2020 Wabi.  All rights reserved.
#
#  CONFIDENTIAL AND COPYRIGHT - FOR WABI INTERNAL USE ONLY - UNAUTHORIZED ACCESS IS STRICTLY
#  PROHIBITED. THIS SOURCE CODE IS PROPRIETARY AND CONFIDENTIAL AND THEREFORE SUBJECT TO THE
#  CONSTITUTIONAL LAWS AND RIGHTS GOVERNING THE UNITED STATES OF AMERICA. ALL RIGHTS, VIEWERSHIP,
#  DISTRIBUTION, POSESSION, AND CONTROL OF THIS SOURCE CODE BELONG SOLEY TO THE COMPANY "WABIXYZ
#  INC."
#
#  IF YOU CAN READ THIS TEXT, THEN YOU ARE IN STRICT VIOLATION OF THE WABI SOFTWARE LICENSE
#  AGREEMENT WHICH IS INCLUDED WITHIN THIS SOFTWARE DISTRIBUTION.
#

# <pep8 compliant>

"""
*** AUTOGENERATED ***
Gives access to KRAKEN data and utility functions.
"""

__all__ = (
    "app"
)

from . import (
    app
)

wg_info = {
    "name": "Kraken Python",
    "author": "Wabi",
    "kraken": (1, 50, 0),
    "description": "Gives access to KRAKEN data and utility functions.",
    "warning": "",
    "doc_url": "https://docs.kraken.xyz/",
    "tracker_url": "",
    "support": 'OFFICIAL',
    "category": "Internal"
}

__title__       = app.name()
__author__      = app.author()
__version__     = app.version()
__description__ = app.description()