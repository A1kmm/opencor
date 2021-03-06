//==============================================================================
// KINSOLSolver plugin
//==============================================================================

#include "kinsolsolver.h"
#include "kinsolsolverplugin.h"

//==============================================================================

namespace OpenCOR {
namespace KINSOLSolver {

//==============================================================================

PLUGININFO_FUNC KINSOLSolverPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("a plugin which uses <a href=\"http://computation.llnl.gov/casc/sundials/description/description.html#descr_kinsol\">KINSOL</a> to solve non-linear systems."));
    descriptions.insert("fr", QString::fromUtf8("une extension qui utilise <a href=\"http://computation.llnl.gov/casc/sundials/description/description.html#descr_kinsol\">KINSOL</a> pour résoudre des systèmes non-linéaires."));

    return new PluginInfo(PluginInfo::InterfaceVersion001,
                          PluginInfo::Simulation,
                          false,
                          true,
                          QStringList() << "CoreSolver" << "SUNDIALS",
                          descriptions);
}

//==============================================================================

Solver::Type KINSOLSolverPlugin::type() const
{
    // Return the type of the solver

    return Solver::Nla;
}

//==============================================================================

QString KINSOLSolverPlugin::name() const
{
    // Return the name of the solver

    return "KINSOL";
}

//==============================================================================

Solver::Properties KINSOLSolverPlugin::properties() const
{
    // Return the properties supported by the solver

    return Solver::Properties();
}

//==============================================================================

void * KINSOLSolverPlugin::instance() const
{
    // Create and return an instance of the solver

    return new KinsolSolver();
}

//==============================================================================

}   // namespace KINSOLSolver
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
