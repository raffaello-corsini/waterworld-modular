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
* 	Questo file contiene la definizione del sistema nelle sue variabili e componenti.
*  Per altre informazioni si rimanda alla relazione allegata al presente progetto.
*
*  Questa è una versione preliminare con 2 flusi d'ingresso, 3 tank e 4 valvole.
*
*  Copyright  2018  Raffaello Corsini, Luca Geretti
*
****************************************************************************/

#ifndef TUTORIAL_SYSTEM_H_
#define TUTORIAL_SYSTEM_H_

#include <ariadne.h>
#include "bottom_tank.h"
#include "side_tank.h"
#include "valve.h"
#include "controller.h"

namespace Ariadne {

  HybridIOAutomaton getSystem()
  {

    // Integer that counts the tanks.
    int tank_counter = 0;
    // Integer that counts the valves.
    int valve_counter = 0;
    // Integer that counts the controllers.
    int controller_counter = 0;

    // 0: System variables

    // Creo le variabili per i livelli d'acqua.

    RealVariable waterLevel0("waterLevel0");
    RealVariable waterLevel1("waterLevel1");
    RealVariable waterLevel2("waterLevel2");

    // RealVariable waterlevel_array[3] = { RealVariable abc("abc"),"b","c"};

    RealVariable valveLevel0("valveLevel0");
    RealVariable valveLevel1("valveLevel1");
    RealVariable valveLevel2("valveLevel2");

    /// Tank automaton

    // 0: Parametri

    // Ingresso dell'entrata della watertank #w0, costante.
    RealParameter w0in("w0in",0.5);
    // Ingresso dell'entrata della watertank #w1, costante.
    RealParameter w1in("w1in",0.5);
    // Uscita delle tank.
    RealParameter tankOutputFlow0("tankOutputFlow0",0.04);
    RealParameter tankOutputFlow1("tankOutputFlow1",0.04);
    RealParameter tankOutputFlow2("tankOutputFlow2",0.04);

    // Provo a creare le due side_tank con la mia funzione.
    HybridIOAutomaton side_tank_0 = Ariadne::getSideTank(
      "side_tank_0",
      waterLevel0,
      valveLevel0,
      valveLevel2,
      w0in,
      tankOutputFlow0,
      // This int represents the number of this tank.
      tank_counter
    );

    tank_counter++;

    HybridIOAutomaton side_tank_1 = Ariadne::getSideTank(
      "side_tank_1",
      waterLevel1,
      valveLevel1,
      valveLevel2,
      w1in,
      tankOutputFlow1,
      // This int represents the number of this tank.
      tank_counter
    );

    tank_counter++;

    // Provo a creare una bottom_tank con la mia funzione.
    HybridIOAutomaton real_bottom_tank = Ariadne::getBottomTank(
      "real_bottom_tank",
      waterLevel2,
      waterLevel0,
      waterLevel1,
      valveLevel2,
      tankOutputFlow0,
      tankOutputFlow1,
      tankOutputFlow2,
      // This int represents the number of this tank.
      tank_counter
    );
      /// Valve automaton

    // 0. Parameters

    RealParameter T("T",4.0); // Time constant for opening/closing the valve

    // 1. Automaton

    // Creo tre valvole con la funzione getValve.

    HybridIOAutomaton external_valve_0 = Ariadne::getValve(
      "external_valve_0",
      // Valve's opening time.
      T,
      // Valve's opening level.
      valveLevel0,
      // This int represents the number of this component.
      valve_counter
    );

    valve_counter++;

    HybridIOAutomaton external_valve_1 = Ariadne::getValve(
      "external_valve_1",
      // Valve's opening time.
      T,
      // Valve's opening level.
      valveLevel1,
      // This int represents the number of this component.
      valve_counter
    );

    valve_counter++;

    HybridIOAutomaton external_valve_2 = Ariadne::getValve(
      "external_valve_2",
      // Valve's opening time.
      T,
      // Valve's opening level.
      valveLevel2,
      // This int represents the number of this component.
      valve_counter
    );

    /// Controller automaton

    // 0. Parameters

    // I parametri li tengo uguali per tutte le tank, visto che le suppongo uguali.
    RealParameter hmin("hmin",5.75); // Lower threshold
    RealParameter hmax("hmax",7.75); // Upper threshold
    RealParameter delta("delta",0.002); // Indetermination constant

    HybridIOAutomaton external_controller_0 = Ariadne::getController(
      "external_controller_0",
      // Controlled tank's waterlevel.
      waterLevel0,
      hmin,hmax,delta,
      // Controlled tank's valve
      external_valve_0,
      // This int represents the number of this component.
      controller_counter
    );

    controller_counter++;

    HybridIOAutomaton external_controller_1 = Ariadne::getController(
      "external_controller_1",
      // Controlled tank's waterlevel.
      waterLevel1,
      hmin,hmax,delta,
      // Controlled tank's valve
      external_valve_1,
      // This int represents the number of this component.
      controller_counter
    );

    controller_counter++;

    HybridIOAutomaton external_controller_2 = Ariadne::getController(
      "external_controller_2",
      // Controlled tank's waterlevel.
      waterLevel2,
      hmin,hmax,delta,
      // Controlled tank's valve
      external_valve_2,
      // This int represents the number of this component.
      controller_counter
    );

    // Compongo la tripletta modulare #0.
    HybridIOAutomaton semimodule0 = compose("side_tank_0,external_valve_0",side_tank_0,external_valve_0,DiscreteLocation("flow0"),DiscreteLocation("idle_0"));
    HybridIOAutomaton module0 = compose("module0",semimodule0,external_controller_0,DiscreteLocation("flow0,idle_0"),DiscreteLocation("rising0"));
    // Compongo la tripletta modulare #1.
    HybridIOAutomaton semimodule1 = compose("side_tank_1,external_valve_1",side_tank_1,external_valve_1,DiscreteLocation("flow1"),DiscreteLocation("idle_1"));
    HybridIOAutomaton module1 = compose("module1",semimodule1,external_controller_1,DiscreteLocation("flow1,idle_1"),DiscreteLocation("rising1"));
    // Compongo la tripletta modulare #0.
    HybridIOAutomaton semimodule2 = compose("real_bottom_tank,external_valve_0",real_bottom_tank,external_valve_2,DiscreteLocation("flow2"),DiscreteLocation("idle_2"));
    HybridIOAutomaton module2 = compose("module2",semimodule2,external_controller_2,DiscreteLocation("flow2,idle_2"),DiscreteLocation("rising2"));
    // Compongo i tre automi.
    HybridIOAutomaton semimodulesystem = compose("semimodulesystem",module0,module1,DiscreteLocation("flow0,idle_0,rising0"),DiscreteLocation("flow1,idle_1,rising1"));
    HybridIOAutomaton modulesystem = compose("modulesystem",semimodulesystem,module2,DiscreteLocation("flow0,idle_0,rising0,flow1,idle_1,rising1"),DiscreteLocation("flow2,idle_2,rising2"));

    // Con questa stampa vedo che l'unica differenza con il vecchio automa composto è il nome.

    ofstream myfile;
    myfile.open ("newfashionsystem.txt");
    myfile << modulesystem;
    myfile.close();

    return modulesystem;

  }

}

#endif /* TUTORIAL_SYSTEM_H_ */
