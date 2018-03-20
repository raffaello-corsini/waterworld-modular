/***************************************************************************
*            system.h
*
*  This file provides the system definition.
*  Specifically, this is watertank system in which a tank with a hole in the
*  bottom receives an input water flow. Such input flow can
*  be modulated between zero and its maximum by controlling a valve. The
*  described controller aims at keeping the water level between an upper threshold
*  and a lower threshold.
*
*  Questo file contiene la definizione del sistema nelle sue variabili e componenti.
*  Per altre informazioni si rimanda alla relazione allegata al presente progetto.
*
*  Questa è una versione preliminare con 2 flussi d'ingresso, 3 tank e 3 valvole.
*
*  Copyright  2018  Raffaello Corsini, Luca Geretti
*
****************************************************************************/

#ifndef TUTORIAL_SYSTEM_H_
#define TUTORIAL_SYSTEM_H_

//#include <utility>      // std::pair, std::make_pair
//#include <iostream>     // std::cout

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

    // Suppore di avere un identico numero
    // di tank, valvole e controller?

    /*
    * Creation of a Vector of Pairs containing an automaton and
    * its starting DiscreteLocation.
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

    /// Tank automaton

    // 0: Parameters.

    // Creation of a vector<RealVariables> for the upper tanks' inputs.
    std::vector<RealParameter> upperflows;

    // Store of the input of the watertank 0, constant.
    upperflows.push_back(RealParameter("w0in",0.5));
    // Store of the input of the watertank 1, constant.
    upperflows.push_back(RealParameter("w1in",0.5));

    // Uscita delle tank.
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

    /// Valve automaton

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
      HybridIOAutomaton controller = Ariadne::getController(
        // Controlled tank's waterlevel.
        waterlevels.at(k),
        hmin,hmax,delta,
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

    /*
    ofstream myfile;
    myfile.open ("newfashionsystem.txt");
    myfile << modulesystem;
    myfile.close();
    */

    return system;

  }

}

#endif /* TUTORIAL_SYSTEM_H_ */
