STM32-Controlled Resistive Heater – Wiring README

Purpose

Drives a heating element (nichrome coil or load resistor) on and off via PWM from an STM32, switched through a logic-level N-channel MOSFET.

Components

-   Power supply: 18.48 V (from barrel jack)
-   Heating element (choose one):
    -   Nichrome coil (~22 Ω), or
    -   Two resistors in series (~12 Ω)
-   N-channel MOSFET: Logic-level (e.g., IRLZ44N)
-   Gate resistor: ~30 Ω (hobby-grade is fine)
-   Pulldown resistor: 10 kΩ (hobby-grade is fine)
-   STM32: PWM output pin

Connections

1.  Connect the power supply positive to the heating element.
2.  Connect the other end of the heating element to the MOSFET Drain.
3.  Connect the MOSFET Source to the power supply negative (ground).
    This is the common ground.
4.  Connect the STM32 PWM output pin through the gate resistor to the
    MOSFET Gate.
5.  Connect the MOSFET Gate to ground through the 10 kΩ pulldown
    resistor (same ground as the Source and power supply negative).

How It Works

The STM32 drives the MOSFET gate high, low, or with a PWM signal through the gate resistor. When the gate is driven high, the MOSFET turns on, allowing current to flow from the power supply, through the heating element, and into the MOSFET to ground, producing heat.

The 10 kΩ pulldown resistor keeps the gate low whenever the STM32 output is floating (such as during reset or boot), ensuring the MOSFET remains off and preventing unintended heating.

Before Powering the Real Load

Perform a burn-in test of the heating element in open air for 10–30 seconds on a non-flammable surface. This verifies that the element does not smoke, fail, or have manufacturing defects before it is installed inside the fridge enclosure.
