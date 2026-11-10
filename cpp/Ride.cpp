#include "Ride.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <utility>

std::string toString(RideStatus status) {
    switch (status) {
        case RideStatus::Requested: return "Requested";
        case RideStatus::Assigned:  return "Assigned";
        case RideStatus::Completed: return "Completed";
        case RideStatus::Cancelled: return "Cancelled";
    }
    return "Unknown";
}

std::string money(double amount) {
    std::ostringstream out;
    out << '$' << std::fixed << std::setprecision(2) << amount;
    return out.str();
}

// ----------------------------------------------------------------- Ride ----
Ride::Ride(std::string rideID, std::string pickup, std::string dropoff,
           double distanceMiles)
    : rideID_(std::move(rideID)),
      pickupLocation_(std::move(pickup)),
      dropoffLocation_(std::move(dropoff)),
      distance_(distanceMiles) {
    if (distanceMiles <= 0.0) {
        throw std::invalid_argument("distance must be positive");
    }
}

double Ride::roundCents(double amount) {
    return std::round(amount * 100.0) / 100.0;
}

double Ride::fare() const { return roundCents(2.00 + 1.25 * distance_); }

std::string Ride::rideType() const { return "Basic"; }

void Ride::rideDetails(std::ostream& out) const {
    out << "  [" << rideID_ << "] " << std::left << std::setw(9) << rideType()
        << pickupLocation_ << " -> " << dropoffLocation_ << "\n"
        << "          " << std::fixed << std::setprecision(1) << distance_
        << " mi | fare " << money(fare()) << " | " << toString(status_);
    if (!driverID_.empty()) out << " | driver " << driverID_;
    out << "\n";
}

void Ride::assignTo(const std::string& driverID) {
    if (status_ != RideStatus::Requested) {
        throw std::logic_error(rideID_ + " is not waiting for a driver");
    }
    driverID_ = driverID;
    status_ = RideStatus::Assigned;
}

void Ride::complete() {
    if (status_ != RideStatus::Assigned) {
        throw std::logic_error(rideID_ + " cannot complete from status " +
                               toString(status_));
    }
    fare_ = fare();          // virtual call: the subclass's pricing is used
    status_ = RideStatus::Completed;
}

void Ride::cancel() {
    if (status_ == RideStatus::Completed) {
        throw std::logic_error(rideID_ + " is already completed");
    }
    status_ = RideStatus::Cancelled;
}

// --------------------------------------------------------- StandardRide ----
double StandardRide::fare() const {
    return roundCents(std::max(7.00, 2.50 + 1.50 * getDistance()));
}

std::string StandardRide::rideType() const { return "Standard"; }

// ---------------------------------------------------------- PremiumRide ----
PremiumRide::PremiumRide(std::string rideID, std::string pickup,
                         std::string dropoff, double distanceMiles,
                         std::string vehicleClass)
    : Ride(std::move(rideID), std::move(pickup), std::move(dropoff),
           distanceMiles),
      vehicleClass_(std::move(vehicleClass)) {}

double PremiumRide::fare() const {
    return roundCents(std::max(15.00, 5.00 + 3.25 * getDistance()));
}

std::string PremiumRide::rideType() const { return "Premium"; }

void PremiumRide::rideDetails(std::ostream& out) const {
    Ride::rideDetails(out);                     // reuse the base output
    out << "          vehicle: " << vehicleClass_ << "\n";
}

// ----------------------------------------------------------- SharedRide ----
SharedRide::SharedRide(std::string rideID, std::string pickup,
                       std::string dropoff, double distanceMiles, int riders)
    : StandardRide(std::move(rideID), std::move(pickup), std::move(dropoff),
                   distanceMiles),
      riders_(std::clamp(riders, 1, 4)) {}

double SharedRide::discount() const { return 0.15 * (riders_ - 1); }

double SharedRide::fare() const {
    // Start from the parent's price, then apply the pool discount.
    return roundCents(StandardRide::fare() * (1.0 - discount()));
}

std::string SharedRide::rideType() const { return "Shared"; }

void SharedRide::rideDetails(std::ostream& out) const {
    StandardRide::rideDetails(out);
    out << "          riders: " << riders_ << " ("
        << static_cast<int>(std::lround(discount() * 100)) << "% pool discount)\n";
}
