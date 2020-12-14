/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

// Include Ponca Core components
#include "Ponca/Core/defines.h"
#include "Ponca/Core/enums.h"
#include "Ponca/Core/basket.h"

#include "Ponca/Core/weightKernel.h"
#include "Ponca/Core/weightFunc.h"

#include "Ponca/Core/plane.h"
#include "Ponca/Core/meanPlaneFit.h"
#include "Ponca/Core/covariancePlaneFit.h"
#include "Ponca/Core/mongePatch.h"

#include "Ponca/Core/sphereFit.h"
#include "Ponca/Core/orientedSphereFit.h"
#include "Ponca/Core/orientedEllipsoidFit.h"
#include "Ponca/Core/mlsSphereFitDer.h"
#include "Ponca/Core/curvature.h"
#include "Ponca/Core/gls.h"

#include "Ponca/Core/curvatureEstimation.h"

// not supported on cuda
#ifndef __CUDACC__
# include "Ponca/Core/unorientedSphereFit.h"
#endif
