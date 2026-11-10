#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Driver.h"
#include "Ride.h"
#include "RideSharingSystem.h"
#include "Rider.h"

namespace {
void section(const char* title) { std::cout << "\n-- " << title << "\n"; }
}  // namespace

int main() {
    std::cout << "=== Ride Sharing System (C++) ===\n";
    RideSharingSystem app;

    Driver& carlos = app.addDriver("D01", "Carlos Ruiz", 4.90, 10);
    app.addDriver("D02", "Ben Ortiz", 4.80, 10);
    app.addDriver("D03", "Aisha Khan", 4.70, 10);
    Rider& maya = app.addRider("U01", "Maya Patel");
    Rider& jordan = app.addRider("U02", "Jordan Lee");

    section("Ride requests (dispatcher assigns least-busy, best-rated driver)");
    struct Request { Rider& rider; std::shared_ptr<Ride> ride; };
    std::vector<Request> requests = {
        {maya, std::make_shared<StandardRide>("R101", "Las Colinas",
                                              "DFW Airport", 9.4)},
        {maya, std::make_shared<PremiumRide>("R102", "Uptown",
                                             "AT&T Stadium", 19.8, "Black SUV")},
        {jordan, std::make_shared<SharedRide>("R103", "Deep Ellum",
                                              "Bishop Arts", 4.2, 3)},
        {maya, std::make_shared<PremiumRide>("R104", "Downtown Dallas",
                                             "Stockyards", 32.4, "Sedan")},
        {jordan, std::make_shared<StandardRide>("R105", "Bishop Arts",
                                                "Klyde Warren Park", 2.2)},
    };
    for (auto& r : requests) {
        Driver& d = app.requestRide(r.rider, r.ride);
        std::cout << "  " << r.ride->getRideID() << " " << r.ride->rideType()
                  << " for " << r.rider.getName() << " -> " << d.getName()
                  << "\n";
    }

    section("Polymorphism: rideDetails() and fare() on a mixed list");
    app.showAllRides(std::cout);

    section("Completing rides and rating drivers");
    app.completeRide("R101", 5);
    app.completeRide("R102", 4);
    app.completeRide("R103", 5);
    app.completeRide("R104", 5);
    app.cancelRide("R105");
    std::cout << "  R101-R104 completed and rated; R105 cancelled\n";

    section("Driver info (getDriverInfo)");
    carlos.getDriverInfo(std::cout);

    section("Rider history (viewRides)");
    maya.viewRides(std::cout);
    jordan.viewRides(std::cout);

    section("Encapsulation checks");
    auto snapshot = carlos.getAssignedRides();   // a copy, not the real list
    snapshot.clear();
    std::cout << "  Cleared a copy of Carlos's rides; he still has "
              << carlos.rideCount() << " rides\n";
    // carlos.assignedRides_.clear();  // compile error: member is private
    try {
        carlos.rate(6);
    } catch (const std::out_of_range& e) {
        std::cout << "  rate(6) rejected: " << e.what() << "\n";
    }
    try {
        app.completeRide("R105", 5);
    } catch (const std::logic_error& e) {
        std::cout << "  completeRide(R105) rejected: " << e.what() << "\n";
    }

    section("Fare report (fare() called through Ride pointers)");
    app.printFareReport(std::cout);
    return 0;
}
