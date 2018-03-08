/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit originating from   *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2014 Stanford University and the Authors.           *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include "ReferenceMBnrgKernels.h"
#include "MBnrgForce.h"
#include "openmm/OpenMMException.h"
#include "openmm/internal/ContextImpl.h"
#include "openmm/reference/RealVec.h"
#include "openmm/reference/ReferencePlatform.h"

using namespace MBnrgPlugin;
using namespace OpenMM;
using namespace std;

static vector<RealVec>& extractPositions(ContextImpl& context) {
    ReferencePlatform::PlatformData* data = reinterpret_cast<ReferencePlatform::PlatformData*>(context.getPlatformData());
    return *((vector<RealVec>*) data->positions);
}

static vector<RealVec>& extractForces(ContextImpl& context) {
    ReferencePlatform::PlatformData* data = reinterpret_cast<ReferencePlatform::PlatformData*>(context.getPlatformData());
    return *((vector<RealVec>*) data->forces);
}

void ReferenceCalcMBnrgForceKernel::initialize(const System& system, const MBnrgForce& force) {

    // Can do nothing here (MB-nrg)
    mbsys_initialized = false;

//    // Initialize bond parameters.
//    int numBonds = force.getNumBonds();
//    particle1.resize(numBonds);
//    particle2.resize(numBonds);
//    length.resize(numBonds);
//    k.resize(numBonds);
//    for (int i = 0; i < numBonds; i++)
//        force.getBondParameters(i, particle1[i], particle2[i], length[i], k[i]);
}

double ReferenceCalcMBnrgForceKernel::execute(ContextImpl& context, bool includeForces, bool includeEnergy) {
    vector<RealVec>& pos = extractPositions(context);
    vector<RealVec>& force = extractForces(context);

    double nmtoang = 10.0;
    std::vector<double> xyz_context(3*pos.size());
    for (size_t i = 0; i < pos.size(); i++) {
      xyz_context[3*i + 0] = pos[i][0] * nmtoang;
      xyz_context[3*i + 1] = pos[i][1] * nmtoang;
      xyz_context[3*i + 2] = pos[i][2] * nmtoang;
    }

    if (!mbsys_initialized) {

      mbnrg_initialize(xyz_context);
      mbsys_initialized = true;
    }

    mbnrg_system.SetSysXyz(xyz_context);
    
    double kcaltokj = 4.184;
    double kcalperAngtokjpernm = kcaltokj*10;
    std::vector<double> grad(3*force.size(),0.0);
    std::cout << "Grad size is " << grad.size() << std::endl; 
    double energy = mbnrg_system.Energy(grad,true) * kcaltokj;
    
    for (size_t i = 0; i < force.size(); i++) {
      force[i][0] = -grad[3*i + 0]*kcalperAngtokjpernm;
      force[i][1] = -grad[3*i + 1]*kcalperAngtokjpernm;
      force[i][2] = -grad[3*i + 2]*kcalperAngtokjpernm;
    }

//    int numBonds = particle1.size();
//    double energy = 0;
//    
//    // Compute the interactions.
//    
//    for (int i = 0; i < numBonds; i++) {
//        int p1 = particle1[i];
//        int p2 = particle2[i];
//        RealVec delta = pos[p1]-pos[p2];
//        RealOpenMM r2 = delta.dot(delta);
//        RealOpenMM r = sqrt(r2);
//        RealOpenMM dr = (r-length[i]);
//        RealOpenMM dr2 = dr*dr;
//        energy += k[i]*dr2*dr2;
//        RealOpenMM dEdR = 4*k[i]*dr2*dr;
//        dEdR = (r > 0) ? (dEdR/r) : 0;
//        force[p1] -= delta*dEdR;
//        force[p2] += delta*dEdR;
//    }
    return energy;
}

void ReferenceCalcMBnrgForceKernel::copyParametersToContext(ContextImpl& context, const MBnrgForce& force) {
//    if (force.getNumBonds() != particle1.size())
//        throw OpenMMException("updateParametersInContext: The number of MBnrg bonds has changed");
//    for (int i = 0; i < force.getNumBonds(); i++) {
//        int p1, p2;
//        force.getBondParameters(i, p1, p2, length[i], k[i]);
//        if (p1 != particle1[i] || p2 != particle2[i])
//            throw OpenMMException("updateParametersInContext: A particle index has changed");
//    }
}

void ReferenceCalcMBnrgForceKernel::mbnrg_initialize(std::vector<double> xyz) {
    std::cout << "MB-nrg System Initialization" << std::endl;
    // hardcoded for water
    size_t nw = xyz.size() / 12;
    for (size_t i = 0; i < nw; i++) {
      std::string name = "h2o";
      std::vector<std::string> at_names(3);
      at_names[0] = "O";
      at_names[1] = "H";
      at_names[2] = "H";
      std::vector<double> coords(9);
      for (size_t j = 0; j < 9; j++) {
        coords[j] = xyz[12*i + j];
      }
      mbnrg_system.AddMonomer(coords, at_names, name);
    } 
    mbnrg_system.Initialize();
}
