/***************************************************************************
*            controller.h
*
*  These file is used to describe the valve of a watertank.
*
*  Copyright  2018  Raffaello Corsini, Luca Geretti
*
****************************************************************************/

/*
#ifndef CONTROLLER_H_
#define CONTROLLER_H_
*/

#include <ariadne.h>

namespace Ariadne {

  // Posso passargli un automa? O meglio passargli solo la variabile?
  HybridIOAutomaton getUrgentController(
    // Controlled tank's waterlevel.
    RealVariable waterlevel,
    // Parameter hmin, hmax, delta for the analysis.
    RealParameter hmin,
    RealParameter hmax,
    // RealParameter delta,
    // Controlled tank's valve
    HybridIOAutomaton valve,
    // This int represents the number of this component.
    int progressive)
    {

      // Conversion of the progressive integer to a String.
      String number = Ariadne::to_string(progressive);

      // 1. Automaton
      HybridIOAutomaton controller("controller" + number);

      // 2. Registration of the input/output variables
      controller.add_input_var(waterlevel);

      // 3. Registration of the events

      // Prendo gli eventi che arrivano dalla valvola.
      set<DiscreteEvent> events = valve.input_events();
      set<DiscreteEvent>::iterator it = events.begin();

      // Recupero gli eventi e li salvo.
      DiscreteEvent close_event = *it;
      controller.add_output_event(close_event);
      it++;
      DiscreteEvent open_event = *it;
      controller.add_output_event(open_event);

      // 4. Registration of the locations
      DiscreteLocation rising("rising" + number);
      DiscreteLocation falling("falling" + number);
      controller.new_mode(rising);
      controller.new_mode(falling);

      // 5. Transitions

      RealExpression waterlevel_geq_hmax = waterlevel - hmax;
      RealExpression waterlevel_leq_hmin = hmin - waterlevel;
      controller.new_forced_transition(close_event, rising, falling, waterlevel_geq_hmax);
      controller.new_forced_transition(open_event, falling, rising, waterlevel_leq_hmin);

      return controller;

    }

  }
