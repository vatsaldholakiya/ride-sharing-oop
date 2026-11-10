#include "RideSharingSystem.h"

#include <iomanip>
#include <ostream>
#include <stdexcept>

Driver& RideSharingSystem::addDriver(const std::string& id,
                                     const std::string& name, double rating,
                                     int ratingsCount) {
    drivers_.push_back(std::make_unique<Driver>(id, name, rating, ratingsCount));
    return *drivers_.back();
}

Rider& RideSharingSystem::addRider(const std::string& id,
                                   const std::string& name) {
    riders_.push_back(std::make_unique<Rider>(id, name));
    return *riders_.back();
}

Driver& RideSharingSystem::pickDriver() {
    if (drivers_.empty()) throw std::runtime_error("no drivers online");
    Driver* best = drivers_.front().get();
    for (const auto& d : drivers_) {
        bool lessBusy = d->activeRideCount() < best->activeRideCount();
        bool sameLoadBetterRating =
            d->activeRideCount() == best->activeRideCount() &&
            d->getRating() > best->getRating();
        if (lessBusy || sameLoadBetterRating) best = d.get();
    }
    return *best;
}

Driver& RideSharingSystem::requestRide(Rider& rider,
                                       const std::shared_ptr<Ride>& ride) {
    rider.requestRide(ride);
    Driver& driver = pickDriver();
    driver.addRide(ride);
    rides_.push_back(ride);
    return driver;
}

std::shared_ptr<Ride> RideSharingSystem::findRide(const std::string& rideID) {
    for (const auto& ride : rides_) {
        if (ride->getRideID() == rideID) return ride;
    }
    throw std::invalid_argument("unknown ride " + rideID);
}

Driver& RideSharingSystem::driverFor(const Ride& ride) {
    for (const auto& d : drivers_) {
        if (d->getDriverID() == ride.getDriverID()) return *d;
    }
    throw std::logic_error(ride.getRideID() + " has no driver");
}

void RideSharingSystem::completeRide(const std::string& rideID, int stars) {
    auto ride = findRide(rideID);
    ride->complete();
    driverFor(*ride).rate(stars);
}

void RideSharingSystem::cancelRide(const std::string& rideID) {
    findRide(rideID)->cancel();
}

void RideSharingSystem::showAllRides(std::ostream& out) const {
    for (const auto& ride : rides_) ride->rideDetails(out);  // dynamic dispatch
}

void RideSharingSystem::printFareReport(std::ostream& out) const {
    double revenue = 0.0;
    out << "  " << std::left << std::setw(6) << "ID" << std::setw(10) << "Type"
        << std::right << std::setw(7) << "Miles" << std::setw(10) << "Fare"
        << "  Status\n";
    for (const auto& ride : rides_) {
        out << "  " << std::left << std::setw(6) << ride->getRideID()
            << std::setw(10) << ride->rideType() << std::right
            << std::setw(7) << std::fixed << std::setprecision(1)
            << ride->getDistance() << std::setw(10) << money(ride->fare())
            << "  " << toString(ride->getStatus()) << "\n";
        revenue += ride->getChargedFare();
    }
    out << "  Revenue from completed rides: " << money(revenue)
        << "  (platform share " << money(revenue * 0.20) << ")\n";
}
