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
* Questo file contiene la definizione del sistema nelle sue variabili e componenti.
*  Per altre informazioni si rimanda alla relazione allegata al presente progetto.
*
*  Questa è una versione preliminare con 2 flussi d'ingresso, 3 tank e 3 valvole.
*
*  Copyright  2018  Raffaello Corsini, Luca Geretti
*
****************************************************************************/

#ifndef TUTORIAL_SYSTEM_H_
#define TUTORIAL_SYSTEM_H_

#include <utility>      // std::pair, std::make_pair
#include <iostream>     // std::cout

#include <ariadne.h>
#include "bottom_tank.h"
#include "side_tank.h"
#include "valve.h"
#include "urgent-valve.h"
#include "controller.h"
#include "automaton-composition.h"

namespace Ariadne {

  HybridIOAutomaton getSystem() {

    // Integer that counts the tanks.
    int tank_counter = 0;
    // Integer that counts the valves.
    int valve_counter = 0;
    // Integer that counts the controllers.
    int controller_counter = 0;

    // Creare un unico component counter,
    // da resettare a zero ogni volta che
    // si passa ad altri componenti.

    // Number of the tanks.
    int tank_number = 3;
    // Number of the valves.
    int valve_number = 3;
    // Number of the controllers.
    int controller_number = 3;

    // Suppore di avere un identico numero
    // di tank, valvole e controller?

    // NOVITÀ! Creo un unico vettore che conterrà tutti gli automi
    // con la relativa locazione di partenza.
    std::vector< pair<HybridIOAutomaton,DiscreteLocation> > mainVector;

    // 0: System variables

    // Creo le variabili per i livelli d'acqua.

    std::vector<RealVariable> waterlevels;

    for (int k = 0; k < tank_number; k++){
      waterlevels.push_back(RealVariable("waterLevel" + Ariadne::to_string(k)));
    }

    // Creo le variabili per l'apertura delle valvole.

    std::vector<RealVariable> valvelevels;

    for (int k = 0; k < valve_number; k++){
      valvelevels.push_back(RealVariable("valveLevel" + Ariadne::to_string(k)));
    }

    /// Tank automaton

    // 0: Parametri

    // Creo il vettore per i parametri di input delle tank superiori.
    std::vector<RealParameter> upperflows;

    // Ingresso dell'entrata della watertank #w0, costante.
    upperflows.push_back(RealParameter("w0in",0.5));
    // Ingresso dell'entrata della watertank #w1, costante.
    upperflows.push_back(RealParameter("w1in",0.5));

    // Uscita delle tank.
    // Creo il vettore per i parametri di uscita.
    std::vector<RealParameter> lowerflows;

    lowerflows.push_back(RealParameter("tankOutputFlow0",0.04));
    lowerflows.push_back(RealParameter("tankOutputFlow1",0.04));
    lowerflows.push_back(RealParameter("tankOutputFlow2",0.04));

    for (int k = 0; k < 2; k++){
      // Provo a creare le due side_tank con la mia funzione.
      HybridIOAutomaton local_tank = Ariadne::getSideTank(
        //"side_tank_0",
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

    // Provo a creare una bottom_tank con la mia funzione.
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

    RealParameter T("T",4.0); // Time constant for opening/closing the valve

    // 1. Automaton

    // Creo tre valvole con la funzione getValve.

    for (int k = 0; k < valve_number; k++){
      HybridIOAutomaton valve = Ariadne::getUrgentValve(
        // Valve's opening time.
        T,
        // Valve's opening level.
        valvelevels.at(k),
        // This int represents the number of this component.
        valve_counter
      );

      pair<HybridIOAutomaton,DiscreteLocation> pair (valve, "idle_" + Ariadne::to_string(k));
      mainVector.push_back(pair);
      valve_counter++;
    }

    /// Controller automaton

    // 0. Parameters

    // I parametri li tengo uguali per tutte le tank, visto che le suppongo uguali.
    RealParameter hmin("hmin",5.75); // Lower threshold
    RealParameter hmax("hmax",7.75); // Upper threshold
    RealParameter delta("delta",0.002); // Indetermination constant

    // 1. Automata

    // Creo tre controllori con la funzione getController.

    for (int k = 0; k < controller_number; k++){

      HybridIOAutomaton controller = Ariadne::getController(
        // Controlled tank's waterlevel.
        waterlevels.at(k),
        hmin,hmax,delta,
        // Controlled tank's valve
        std::get<0>(mainVector.at(tank_number + k)),
        // This int represents the number of this component.
        controller_counter
      );
      pair<HybridIOAutomaton,DiscreteLocation> pair (controller, "rising" + Ariadne::to_string(k));
      mainVector.push_back(pair);
      controller_counter++;
    }

    // Compongo tutti gli automi per ottenere un unico sistema.
    HybridIOAutomaton system = composition_all_pieces_together(mainVector);

    /*
    ofstream myfile;
    myfile.open ("newfashionsystem.txt");
    myfile << modulesystem;
    myfile.close();
    */

    // Ritorno il sistema.
    return system;

  }

}

#endif /* TUTORIAL_SYSTEM_H_ */
