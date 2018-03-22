/***************************************************************************
*            system.h
*
*  This file contains a function providing the desired system.
*  Specifically, this is a watertank system with two side_tank on the top,
*  one bottom_tank at the bottom and a valve on top of each of them.
*  For every valve there is also a controller regulating its opening
*  and closing.
*
*  Copyright  2018  Raffaello Corsini, Luca Geretti
*
****************************************************************************/

/*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Library General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <ariadne.h>
#include "bottom_tank.h"
#include "side_tank.h"
#include "valve.h"
#include "urgent-controller.h"
#include "controller.h"
#include "automaton-composition.h"

namespace Ariadne {

  HybridIOAutomaton getSystem() {

    // Integer that counts the tanks.
    int tank_counter = 0;

    // Number of the tanks.
    int tank_number = 3;
    // Number of the valves.
    int valve_number = 3;
    // Number of the controllers.
    int controller_number = 3;

    /*
    * Creation of a Vector of Pairs containing
    * an automaton and its starting DiscreteLocation.
    */
    std::vector< pair<HybridIOAutomaton,DiscreteLocation> > mainVector;

    // 0: System variables

    // Creation of the RealVariables for the tanks' waterlevels.
    std::vector<RealVariable> waterlevels;
    for (int k = 0; k < tank_number; k++){
      waterlevels.push_back(RealVariable("waterLevel" + Ariadne::to_string(k)));
    }

    // Creation of the RealVariables for the valves' opening levels..
    std::vector<RealVariable> valvelevels;
    for (int k = 0; k < valve_number; k++){
      valvelevels.push_back(RealVariable("valveLevel" + Ariadne::to_string(k)));
    }

    /// Tank automata

    // 0: Parameters.

    // Creation of a vector<RealVariables> for the upper tanks' inputs.
    std::vector<RealParameter> upperflows;

    // Store of the input of the watertank 0, constant.
    upperflows.push_back(RealParameter("w0in",0.5));
    // Store of the input of the watertank 1, constant.
    upperflows.push_back(RealParameter("w1in",0.5));

    // Creation of a vector<RealParameter> for the tanks' output diameter.
    std::vector<RealParameter> lowerflows;

    // Store of the three parameters, one for each tank.
    lowerflows.push_back(RealParameter("tankOutputFlow0",0.04));
    lowerflows.push_back(RealParameter("tankOutputFlow1",0.04));
    lowerflows.push_back(RealParameter("tankOutputFlow2",0.04));

    // Creation of the two top tanks.
    for (int k = 0; k < 2; k++){
      HybridIOAutomaton local_tank = Ariadne::getSideTank(
        waterlevels.at(k),
        valvelevels.at(k),
        valvelevels.at(2),
        upperflows.at(k),
        lowerflows.at(k),
        // This int represents the number of this tank.
        tank_counter
      );
      pair<HybridIOAutomaton,DiscreteLocation> pair (local_tank, "flow" + Ariadne::to_string(k));
      mainVector.push_back(pair);
      tank_counter++;
    }

    // Creation of the bottom tank.
    HybridIOAutomaton real_bottom_tank = Ariadne::getBottomTank(
      waterlevels.at(2),
      waterlevels.at(0),
      waterlevels.at(1),
      valvelevels.at(2),
      lowerflows.at(0),
      lowerflows.at(1),
      lowerflows.at(2),
      // This int represents the number of this tank.
      tank_counter
    );

    pair<HybridIOAutomaton,DiscreteLocation> bottom_tank_pair (real_bottom_tank, DiscreteLocation("flow2"));
    mainVector.push_back(bottom_tank_pair);

    /// Valve automata

    // 0. Parameters

    // Time constant for opening/closing the valves.
    RealParameter T("T",4.0);

    // 1. Automaton

    // Creation of three valves.
    for (int k = 0; k < valve_number; k++){
      HybridIOAutomaton valve = Ariadne::getValve(
        // Valve's opening time.
        T,
        // Valve's opening level.
        valvelevels.at(k),
        // This int represents the number of this component.
        k
      );
      pair<HybridIOAutomaton,DiscreteLocation> pair (valve, "idle_" + Ariadne::to_string(k));
      mainVector.push_back(pair);
    }

    /// Controller automaton

    // 0. Parameters

    /*
    * The parameters checked by the controllers.
    * In this version we considerd them identical for every tank.
    */
    RealParameter hmin("hmin",5.75); // Lower threshold
    RealParameter hmax("hmax",7.75); // Upper threshold
    RealParameter delta("delta",0.002); // Indetermination constant

    // 1. Automata

    // Creation of three controllers.
    for (int k = 0; k < controller_number; k++){
      HybridIOAutomaton controller = Ariadne::getUrgentController(
        // Controlled tank's waterlevel.
        waterlevels.at(k),
        hmin,hmax,//delta,
        // Controlled tank's valve
        std::get<0>(mainVector.at(tank_number + k)),
        // This int represents the number of this component.
        k
      );
      pair<HybridIOAutomaton,DiscreteLocation> pair (controller, "rising" + Ariadne::to_string(k));
      mainVector.push_back(pair);
    }

    // Composition of all the automata in order to get a single one.
    HybridIOAutomaton system = composition_all_pieces_together(mainVector);

    return system;

  }

}
