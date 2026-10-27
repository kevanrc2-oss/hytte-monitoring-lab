# Assignment 2 – initial demo scope

## Working foundation

Verified manually:
- The firmware builds through GitHub Actions.
- The micro:bit displays R at startup.
- Button A changes the display to A.
- The dashboard receives BUTTON_A through Web Serial.
- The dashboard can close the serial connection.

## Fixed acceptance requirements for the cabin demo

| ID | Requirement |
|---|---|
| R01 | The user selects HOME or AWAY before monitoring starts. |
| R02 | Door activity in HOME creates no security Warning. |
| R03 | Enabled Door monitoring in AWAY creates a Warning. |
| R04 | Door monitoring can be disabled. |
| R05 | The critical Smoke scenario creates a Critical Incident in HOME and AWAY. |
| R06 | Resolved closes the existing Incident without changing its ID. |
| R07 | Not Resolved keeps the Incident Active. |
| R08 | The browser stores Incident history and retains it after a page reload. |

## Figure of Merit

FoM = verified acceptance requirements / 8 × 100%.

The same eight requirements will be used throughout this measurement series.
This measures the selected demo scope, not the complete system model.

## Baseline

Verified cabin-demo requirements: 0/8.
FoM: 0%.

The hardware and serial foundation works, but the cabin behavior above
has not been implemented.

This baseline is based on code inspection.
Automated acceptance tests have not yet been added.

## Measurement 2 – door monitoring

Commit: 731c0fb
Verification method: manual test on micro:bit and dashboard.

- R01 verified: A before mode selection returns ERROR MODE_NOT_SELECTED.
- R02 verified: Door activity in HOME returns DOOR IGNORED.
- R03 verified: Door activity in AWAY creates an Active Warning.

Additional check:
Repeated A presses retain the same active Incident ID.

Verified requirements: 3/8.
FoM: 37.5%.

R04–R08 have not yet been verified.
Automated acceptance tests have not yet been added.