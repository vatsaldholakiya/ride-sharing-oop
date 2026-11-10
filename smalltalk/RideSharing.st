"=====================================================================
 Ride Sharing System - class definitions (GNU Smalltalk 3.2.x)
 Run with:  gst RideSharing.st main.st
 ====================================================================="

"Small formatting helpers added to existing library classes."
Number extend [
    printFixed: places [
        "4.916 printFixed: 2 -> '4.92'"
        | scale scaled |
        scale := 10 raisedTo: places.
        scaled := (self * scale) rounded.
        ^(scaled // scale) printString, '.',
            ((scaled \\ scale) printPaddedWith: $0 to: places)
    ]
    asMoney [ ^'$', (self printFixed: 2) ]
]

String extend [
    padRight: width [ ^self, (String new: (width - self size max: 0) withAll: $ ) ]
    padLeft: width [ ^(String new: (width - self size max: 0) withAll: $ ), self ]
]

"---------------------------------------------------------------- Ride"
Object subclass: Ride [
    | rideID pickupLocation dropoffLocation distance fare status driverID |
    <comment: 'Base ride. Instance variables are private to the object:
other objects can only send it messages.'>

    Ride class >> id: anID from: pickup to: dropoff miles: miles [
        miles > 0 ifFalse: [^self error: 'distance must be positive'].
        ^self new setID: anID from: pickup to: dropoff miles: miles
    ]

    setID: anID from: pickup to: dropoff miles: miles [
        <category: 'private'>
        rideID := anID.
        pickupLocation := pickup.
        dropoffLocation := dropoff.
        distance := miles.
        fare := 0.                  "amount charged; set by #complete"
        status := #Requested
    ]

    "Read-only accessors"
    rideID [ ^rideID ]
    pickupLocation [ ^pickupLocation ]
    dropoffLocation [ ^dropoffLocation ]
    distance [ ^distance ]
    driverID [ ^driverID ]
    status [ ^status ]
    chargedFare [ ^fare ]

    roundCents: amount [ ^(amount * 100) rounded / 100 ]

    "Polymorphic interface - subclasses override these"
    fare [ ^self roundCents: 2 + (1.25 * distance) ]      "$2.00 + $1.25/mi"
    rideType [ ^'Basic' ]

    rideDetails [
        Transcript
            showCr: '  [', rideID, '] ', (self rideType padRight: 9),
                pickupLocation, ' -> ', dropoffLocation;
            show: '          ', (distance printFixed: 1), ' mi | fare ',
                self fare asMoney, ' | ', status asString.
        driverID isNil ifFalse: [Transcript show: ' | driver ', driverID].
        Transcript cr
    ]

    "Lifecycle: Requested -> Assigned -> Completed (or Cancelled)"
    assignTo: aDriverID [
        status == #Requested
            ifFalse: [^self error: rideID, ' is not waiting for a driver'].
        driverID := aDriverID.
        status := #Assigned
    ]

    complete [
        status == #Assigned ifFalse: [
            ^self error: rideID, ' cannot complete from status ', status].
        fare := self fare.          "dynamic dispatch picks the subclass price"
        status := #Completed
    ]

    cancel [
        status == #Completed
            ifTrue: [^self error: rideID, ' is already completed'].
        status := #Cancelled
    ]
]

"-------------------------------------------------------- StandardRide"
Ride subclass: StandardRide [
    <comment: 'Everyday car: $2.50 + $1.50/mi, $7.00 minimum.'>

    fare [ ^self roundCents: (7 max: 2.5 + (1.5 * distance)) ]
    rideType [ ^'Standard' ]
]

"--------------------------------------------------------- PremiumRide"
Ride subclass: PremiumRide [
    | vehicleClass |
    <comment: 'Luxury vehicle: $5.00 + $3.25/mi, $15.00 minimum.'>

    PremiumRide class >> id: anID from: pickup to: dropoff miles: miles
            vehicle: aVehicleClass [
        ^(self id: anID from: pickup to: dropoff miles: miles)
            setVehicle: aVehicleClass
    ]

    setVehicle: aVehicleClass [
        <category: 'private'>
        vehicleClass := aVehicleClass
    ]

    fare [ ^self roundCents: (15 max: 5 + (3.25 * distance)) ]
    rideType [ ^'Premium' ]

    rideDetails [
        super rideDetails.                      "reuse the base output"
        Transcript showCr: '          vehicle: ', vehicleClass
    ]
]

"---------------------------------------------------------- SharedRide"
StandardRide subclass: SharedRide [
    | riders |
    <comment: 'Pooled ride: a StandardRide discounted 15% per extra rider.'>

    SharedRide class >> id: anID from: pickup to: dropoff miles: miles
            riders: count [
        ^(self id: anID from: pickup to: dropoff miles: miles)
            setRiders: count
    ]

    setRiders: count [
        <category: 'private'>
        riders := (count max: 1) min: 4
    ]

    discount [ ^(riders - 1) * 15 / 100 ]

    fare [
        "Start from the parent's price, then apply the pool discount."
        ^self roundCents: super fare * (1 - self discount)
    ]

    rideType [ ^'Shared' ]

    rideDetails [
        super rideDetails.
        Transcript showCr: '          riders: ', riders printString, ' (',
            (self discount * 100) printString, '% pool discount)'
    ]
]

"-------------------------------------------------------------- Driver"
Object subclass: Driver [
    | driverID name rating ratingsCount assignedRides |

    Driver class >> id: anID name: aName rating: aRating ratings: count [
        ^self new setID: anID name: aName rating: aRating ratings: count
    ]

    setID: anID name: aName rating: aRating ratings: count [
        <category: 'private'>
        driverID := anID.
        name := aName.
        rating := aRating.
        ratingsCount := count.
        assignedRides := OrderedCollection new
    ]

    driverID [ ^driverID ]
    name [ ^name ]
    rating [ ^rating ]
    payoutShare [ ^4 / 5 ]                       "platform keeps 20%"

    "assignedRides has no accessor; these are the only ways to reach it"
    addRide: aRide [
        aRide assignTo: driverID.
        assignedRides add: aRide
    ]
    rideCount [ ^assignedRides size ]
    activeRideCount [ ^(assignedRides select: [:r | r status == #Assigned]) size ]
    ridesSnapshot [ ^assignedRides copy ]       "a copy, never the original"

    earnings [
        ^(assignedRides inject: 0 into: [:sum :r | sum + r chargedFare])
            * self payoutShare
    ]

    rate: stars [
        (stars between: 1 and: 5)
            ifFalse: [^self error: 'rating must be between 1 and 5'].
        rating := (rating * ratingsCount + stars) / (ratingsCount + 1).
        ratingsCount := ratingsCount + 1
    ]

    getDriverInfo [
        Transcript showCr: '  ', name, ' (', driverID, ')  rating ',
            (rating printFixed: 2), '  rides ', assignedRides size printString,
            '  earnings ', self earnings asMoney.
        assignedRides do: [:r |
            Transcript showCr: '      - ', r rideID, ' ', r rideType, ', ',
                r status asString]
    ]
]

"--------------------------------------------------------------- Rider"
Object subclass: Rider [
    | riderID name requestedRides |

    Rider class >> id: anID name: aName [
        ^self new setID: anID name: aName
    ]

    setID: anID name: aName [
        <category: 'private'>
        riderID := anID.
        name := aName.
        requestedRides := OrderedCollection new
    ]

    riderID [ ^riderID ]
    name [ ^name ]

    requestRide: aRide [ requestedRides add: aRide ]

    totalSpent [
        ^requestedRides inject: 0 into: [:sum :r | sum + r chargedFare]
    ]

    viewRides [
        Transcript showCr: '  ', name, ' (', riderID, ') - ',
            requestedRides size printString, ' rides, spent ',
            self totalSpent asMoney.
        requestedRides do: [:r | r rideDetails]   "polymorphic send"
    ]
]

"--------------------------------------------------- RideSharingSystem"
Object subclass: RideSharingSystem [
    | drivers riders rides |
    <comment: 'Dispatcher. rides holds every ride, whatever its class.'>

    RideSharingSystem class >> new [ ^super new init ]

    init [
        <category: 'private'>
        drivers := OrderedCollection new.
        riders := OrderedCollection new.
        rides := OrderedCollection new
    ]

    addDriver: aDriver [ ^drivers add: aDriver ]
    addRider: aRider [ ^riders add: aRider ]

    pickDriver [
        "Least-busy driver; ties go to the higher rating."
        | best |
        drivers isEmpty ifTrue: [^self error: 'no drivers online'].
        best := drivers first.
        drivers do: [:d |
            (d activeRideCount < best activeRideCount
                or: [d activeRideCount = best activeRideCount
                    and: [d rating > best rating]])
                ifTrue: [best := d]].
        ^best
    ]

    requestRide: aRide for: aRider [
        | driver |
        aRider requestRide: aRide.
        driver := self pickDriver.
        driver addRide: aRide.
        rides add: aRide.
        ^driver
    ]

    findRide: anID [
        ^rides detect: [:r | r rideID = anID]
            ifNone: [self error: 'unknown ride ', anID]
    ]

    driverFor: aRide [ ^drivers detect: [:d | d driverID = aRide driverID] ]

    completeRide: anID stars: stars [
        | ride |
        ride := self findRide: anID.
        ride complete.
        (self driverFor: ride) rate: stars
    ]

    cancelRide: anID [ (self findRide: anID) cancel ]

    showAllRides [ rides do: [:r | r rideDetails] ]

    printFareReport [
        | revenue |
        revenue := 0.
        Transcript showCr: '  ', ('ID' padRight: 6), ('Type' padRight: 10),
            ('Miles' padLeft: 7), ('Fare' padLeft: 10), '  Status'.
        rides do: [:r |
            Transcript showCr: '  ', (r rideID padRight: 6),
                (r rideType padRight: 10),
                ((r distance printFixed: 1) padLeft: 7),
                (r fare asMoney padLeft: 10), '  ', r status asString.
            revenue := revenue + r chargedFare].
        Transcript showCr: '  Revenue from completed rides: ', revenue asMoney,
            '  (platform share ', (revenue / 5) asMoney, ')'
    ]
]
