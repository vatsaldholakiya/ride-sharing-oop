# Ride Sharing System — OOP in C++ and Smalltalk

MSCS 632 assignment: the same class-based ride sharing system written in
C++17 and GNU Smalltalk, demonstrating encapsulation, inheritance and
polymorphism.

## Design

| Class | Role |
|---|---|
| `Ride` | Base class: ride ID, pickup, dropoff, distance, charged fare, status, driver. `fare()` and `rideDetails()` are overridable. |
| `StandardRide` | $2.50 + $1.50/mi, $7.00 minimum |
| `PremiumRide` | $5.00 + $3.25/mi, $15.00 minimum, adds a vehicle class |
| `SharedRide` | Subclass of `StandardRide`; 15% off per extra rider (max 45%) |
| `Driver` | Private `assignedRides`; `addRide`, `getDriverInfo`, `rate`, earnings (80% payout) |
| `Rider` | Private `requestedRides`; `requestRide`, `viewRides`, total spent |
| `RideSharingSystem` | Dispatcher: assigns the least-busy, best-rated driver; completes, cancels and reports on rides |

Rides move through `Requested → Assigned → Completed` (or `Cancelled`); the
fare is locked in on completion by a polymorphic call to `fare()`.

## Run

**C++** (g++ 9+ or clang++ with C++17):

```sh
cd cpp
make run
```

**Smalltalk** (GNU Smalltalk 3.2+):

```sh
cd smalltalk
gst -g RideSharing.st main.st
```

Both programs print the same scenario, so their outputs can be compared
line by line.

## Layout

```
cpp/        Ride.h/.cpp, Driver.h/.cpp, Rider.h/.cpp,
            RideSharingSystem.h/.cpp, main.cpp, Makefile
smalltalk/  RideSharing.st (classes), main.st (demo script)
```
