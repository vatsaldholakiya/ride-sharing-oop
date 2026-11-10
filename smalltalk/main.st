"Ride Sharing System - demo script.  Run:  gst RideSharing.st main.st"

| section app carlos maya jordan requests snapshot |

section := [:title | Transcript cr; showCr: '-- ', title].
Transcript showCr: '=== Ride Sharing System (Smalltalk) ==='.

app := RideSharingSystem new.
carlos := app addDriver: (Driver id: 'D01' name: 'Carlos Ruiz' rating: 4.9 ratings: 10).
app addDriver: (Driver id: 'D02' name: 'Ben Ortiz' rating: 4.8 ratings: 10).
app addDriver: (Driver id: 'D03' name: 'Aisha Khan' rating: 4.7 ratings: 10).
maya := app addRider: (Rider id: 'U01' name: 'Maya Patel').
jordan := app addRider: (Rider id: 'U02' name: 'Jordan Lee').

section value: 'Ride requests (dispatcher assigns least-busy, best-rated driver)'.
requests := {
    {maya. StandardRide id: 'R101' from: 'Las Colinas' to: 'DFW Airport' miles: 9.4}.
    {maya. PremiumRide id: 'R102' from: 'Uptown' to: 'AT&T Stadium' miles: 19.8
        vehicle: 'Black SUV'}.
    {jordan. SharedRide id: 'R103' from: 'Deep Ellum' to: 'Bishop Arts' miles: 4.2
        riders: 3}.
    {maya. PremiumRide id: 'R104' from: 'Downtown Dallas' to: 'Stockyards' miles: 32.4
        vehicle: 'Sedan'}.
    {jordan. StandardRide id: 'R105' from: 'Bishop Arts' to: 'Klyde Warren Park'
        miles: 2.2}}.
requests do: [:req |
    | rider ride driver |
    rider := req first.
    ride := req last.
    driver := app requestRide: ride for: rider.
    Transcript showCr: '  ', ride rideID, ' ', ride rideType, ' for ',
        rider name, ' -> ', driver name].

section value: 'Polymorphism: rideDetails and fare on a mixed collection'.
app showAllRides.

section value: 'Completing rides and rating drivers'.
app completeRide: 'R101' stars: 5.
app completeRide: 'R102' stars: 4.
app completeRide: 'R103' stars: 5.
app completeRide: 'R104' stars: 5.
app cancelRide: 'R105'.
Transcript showCr: '  R101-R104 completed and rated; R105 cancelled'.

section value: 'Driver info (getDriverInfo)'.
carlos getDriverInfo.

section value: 'Rider history (viewRides)'.
maya viewRides.
jordan viewRides.

section value: 'Encapsulation checks'.
snapshot := carlos ridesSnapshot.             "a copy, not the real list"
snapshot removeAll: snapshot copy.
Transcript showCr: '  Emptied a copy of Carlos''s rides; he still has ',
    carlos rideCount printString, ' rides'.
[carlos assignedRides]
    on: MessageNotUnderstood
    do: [:e | Transcript showCr: '  carlos assignedRides -> doesNotUnderstand: #',
            e message selector.
        e return: nil].
[carlos rate: 6]
    on: Error
    do: [:e | Transcript showCr: '  rate: 6 rejected: ', e messageText.
        e return: nil].
[app completeRide: 'R105' stars: 5]
    on: Error
    do: [:e | Transcript showCr: '  completeRide: R105 rejected: ', e messageText.
        e return: nil].

section value: 'Fare report (fare sent to every ride, whatever its class)'.
app printFareReport.
Transcript flush.
