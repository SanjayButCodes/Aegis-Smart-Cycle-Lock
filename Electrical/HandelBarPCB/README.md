# Handlebar Hub — KiCad 8 Design Package

## Revision B — active design (60 × 40 mm)

`Handlebar_Hub_revB.kicad_pcb` is the active, collision-free placement. It supersedes the original 40 × 40 mm fit-study board, which is retained only as a record of the rejected constraints.

The board outline is X=0…60 mm and Y=0…40 mm. The four 3.2 mm NPTH mounting holes are exactly at (3,3), (57,3), (3,37), and (57,37) mm: precisely 3 mm in from every corner. A1 is rotated so its 45.72 mm DIP pin-row dimension runs along the 60 mm board length. J1 is now a 6-pin JST-XH locking header, placed in the upper service band; J2 is upper-left, J3 lower-left, D1 lower-right, and R1–R3 are directly left of D1.

The 0.25 mm B.Cu tamper trace uses J1.5 and J1.6 and follows the entire perimeter 0.8 mm inside the Edge.Cuts. It passes between the board edge and each 3 mm-inset mounting hole. This leaves roughly 0.48 mm copper-to-hole clearance, not the prior 1 mm recommendation; that reduced clearance is unavoidable with the requested perimeter-loop and hole geometry. Confirm that this meets the fabricator's NPTH copper-clearance rule.

## Revision A status: electrically defined; mechanically **not feasible as specified**

This package intentionally does not pretend that the requested parts fit. `Handlebar_Hub.kicad_pcb` is a native KiCad 8 mechanical fit-study board; it is **not a fabrication-ready PCB**. The Arduino Nano footprint is placed outside the 40 mm outline as a witness of the conflict. J1 is intentionally drawn colliding with H1/H2.

### Blocking mechanical conflicts

| Requirement | Geometry | Result |
|---|---:|---|
| Arduino Nano DIP-30 | 45.72 mm pin-row span (about 47 mm module length) | Cannot be contained in a 40 mm board in either orientation, before edge clearance. |
| Four 3.2 mm holes at (3,3), (37,3), (3,37), (37,37) | 34 mm center-to-center along an edge | Hole annuli occupy the terminal-block end zones. |
| 6-way 5.08 mm terminal | 25.40 mm first-to-last pad pitch; roughly 31–34 mm body | Its real body/entry clearance clashes with the corner holes on a 40 mm edge. |
| All three together | Useful central envelope is much smaller than 40 mm | No routable/assembly-safe arrangement exists. |

### Required decision before manufacturing

Choose one: (1) enlarge the PCB to at least 55 x 50 mm and move holes/connector clearances accordingly; (2) move the Nano and/or bridge terminal off-board; or (3) replace A1 with a compact on-board ATmega328P/5 V module and use a smaller connector. A 50 x 50 mm board may fit a Nano only with very tight edge/connector compromises; 55 x 50 mm is the conservative starting point.

## Deliverables

* `Handlebar_Hub.sch` — import-compatible legacy Eeschema schematic. Open in KiCad 8 and **Save As** to convert it to native `.kicad_sch`.
* `Handlebar_Hub.net` — complete functional netlist with requested library footprint assignments.
* `Handlebar_Hub_revB.kicad_pcb` — active 60 x 40 mm two-layer placement and routed design.
* `Handlebar_Hub.kicad_pcb` — superseded 40 x 40 fit study, retained as the collision witness.
* `Handlebar_Hub.kicad_dru` — custom KiCad rules. Board Setup → Design Rules → Import/Load as appropriate for the installed KiCad version.

## Functional net assignment

| Function | Nano | Destination |
|---|---|---|
| 5 V / ground | 5V / both GND pins | J1.1/J1.2 and J2.1/J2.2 |
| UART | D1 TX / D0 RX | J1.3 / J1.4 (no crossover on this board) |
| I²C | A4 SDA / A5 SCL | J2.3 / J2.4 |
| Button | D2 | J3.1; J3.2 is GND; firmware enables pull-up |
| RGB | D3 / D5 / D6 | R1/R2/R3 220 Ω then red/green/blue anodes; common cathode GND |
| Tamper | J1.5 and J1.6 | `TAMPER_LOOP` copper perimeter loop; rear hub must measure continuity/impedance |

## Layout constraints for a revised mechanically feasible board

* 2 layers, 1.6 mm FR-4; F.Cu signal/component routing and near-solid B.Cu GND plane. Stitch GND at connector, Nano, NFC, and LED area.
* Default clearance 0.20 mm and signal width 0.25 mm. Use at least 0.80 mm for the incoming 5 V/GND trunks; size larger after confirming worst-case NFC + LED + Nano current and connector rating.
* Place J1 at an accessible edge, NFC header away from the UART/LED switching paths, button connector adjacent to its cable exit, and LED at the optical edge. Keep I²C short and together; do not route it under a switching LED trace.
* Tamper loop: B.Cu, 0.25 mm, 0.8 mm from Edge.Cuts; keep 0.50 mm clearance from other copper and no vias/test pads/stitching in the loop. The requested hole geometry forces the loop between the board edge and each hole, with about 0.48 mm NPTH clearance. Any copper break must make J1.5–J1.6 open.
* Do not make a copper zone on the tamper layer that could bypass the loop. Confirm rear-hub detection topology: the present interpretation shorts J1.5 to J1.6 through one continuous monitored conductor.

## Electrical assumptions and checks still required

* The external bridge supplies a regulated 5 V and shares ground. No reverse-polarity, fuse, ESD, TVS, or local decoupling parts were specified; add them for a vehicle cable environment.
* PN532 boards vary: confirm the selected board is 5 V tolerant in I²C mode and whether it already includes pull-ups. If not, add suitable pull-ups to 5 V.
* The listed 220 Ω LED resistors are electrically consistent only for a low-current indicator LED. Verify each RGB die forward voltage/current and Nano per-pin/total current limits; blue at 5 V can be too bright/current-heavy depending on the LED.
* Confirm the Arduino Nano library pin numbers against the exact installed footprint before importing netlist; the supplied mapping follows the standard Nano DIP-30 convention: D1=1, D0=2, D2=5, D3=6, D5=8, D6=9, A5=17, A4=18, 5V=27, GND=4/29.
