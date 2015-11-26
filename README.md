# Arduino Heater

This is a small arduino project for a heater-box. 
The purpose of this project is to pre-warm lithium polymer batteries with high discharge rates (25C+). The advantage is that lipos have a way lower inner resistence (Ri)the warmer they get. The lower Ri is, the lower the voltage drop on high loads.

**The shopping list:**
1x arduino (I use a leonardo board)
2x LM35 (thermal resistor)
1x heating unit (I use a 36W heatingfoil)
1x 20x4 LCD Display
Resistores:
1x 47k
1x 22k
1x 180
1x 10k


This state mashine has four states:

  - Warmup
  - LiPo cold
  - LiPo warm
  - Vin critical

**Warmup:**

This is the initial state. Box is cold and gets warmed up with full power until the heating unit reaches 40°C. The hysterese for this state is 0.5°C. When the heating unit reaches 40°C and the state switches to "LiPo cold" (for batteries being below 35°C) or "LiPo warm" (for batteries being at at least 35°C)

**LiPo cold:**

In this state the target temperature of the heating unit is still 40°C but the heating power gets regulated depending on the temperature delta. This state switches to "LiPo warm" if the batteries reach at least 35°C

**LiPo warm**

In this state is the same as "LiPo cold" but the target temperature of the heating unit is 36°C. If the batteries are getting below 32°C we switch back to "LiPo cold"

**Vin critical**

Whenever the powersource gets below a critical voltage (10.9 volts for a 3S LiPo as power source) the state mashine switches to "vin critical" and the heating units powers off.



License
----

**Free Software, Hell Yeah!**
