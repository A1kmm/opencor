//==============================================================================
// SecondOrderRungeKuttaSolver plugin
//==============================================================================

#include "secondorderrungekuttasolver.h"
#include "secondorderrungekuttasolverplugin.h"

//==============================================================================

namespace OpenCOR {
namespace SecondOrderRungeKuttaSolver {

//==============================================================================

PLUGININFO_FUNC SecondOrderRungeKuttaSolverPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("a plugin which implements the <a href=\"http://en.wikipedia.org/wiki/Runge–Kutta_methods\">second-order Runge-Kutta method</a> to solve ODEs."));
    descriptions.insert("fr", QString::fromUtf8("une extension qui implémente la <a href=\"http://en.wikipedia.org/wiki/Runge–Kutta_methods\">méthode Runge-Kutta du deuxième ordre</a> pour résoudre des EDOs."));

    return new PluginInfo(PluginInfo::InterfaceVersion001,
                          PluginInfo::Simulation,
                          false,
                          true,
                          QStringList() << "CoreSolver",
                          descriptions);
}

//==============================================================================

Solver::Type SecondOrderRungeKuttaSolverPlugin::type() const
{
    // Return the type of the solver

    return Solver::Ode;
}

//==============================================================================

QString SecondOrderRungeKuttaSolverPlugin::name() const
{
    // Return the name of the solver

    return "Runge-Kutta (2nd order)";
}

//==============================================================================

Solver::Properties SecondOrderRungeKuttaSolverPlugin::properties() const
{
    // Return the properties supported by the solver

    Solver::Properties res = Solver::Properties();
    Descriptions stepPropertyDescriptions;

    stepPropertyDescriptions.insert("en", QString::fromUtf8("Step"));
    stepPropertyDescriptions.insert("fr", QString::fromUtf8("Pas"));

    res.append(Solver::Property(Solver::Double, StepId, stepPropertyDescriptions, StepDefaultValue, true));

    return res;
}

//==============================================================================

void * SecondOrderRungeKuttaSolverPlugin::instance() const
{
    // Create and return an instance of the solver

    return new SecondOrderRungeKuttaSolver();
}

//==============================================================================

}   // namespace SecondOrderRungeKuttaSolver
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
