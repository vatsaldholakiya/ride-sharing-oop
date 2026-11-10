#include "Driver.h"

#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <utility>

Driver::Driver(std::string driverID, std::string name, double rating,
               int ratingsCount)
    : driverID_(std::move(driverID)),
      name_(std::move(name)),
      rating_(rating),
      ratingsCount_(ratingsCount) {}

void Driver::addRide(const std::shared_ptr<Ride>& ride) {
    if (!ride) throw std::invalid_argument("cannot add an empty ride");
    ride->assignTo(driverID_);
    assignedRides_.push_back(ride);
}

void Driver::rate(int stars) {
    if (stars < 1 || stars > 5) {
        throw std::out_of_range("rating must be between 1 and 5");
    }
    rating_ = (rating_ * ratingsCount_ + stars) / (ratingsCount_ + 1);
    ++ratingsCount_;
}

std::size_t Driver::activeRideCount() const {
    std::size_t active = 0;
    for (const auto& ride : assignedRides_) {
        if (ride->getStatus() == RideStatus::Assigned) ++active;
    }
    return active;
}

double Driver::earnings() const {
    double total = 0.0;
    for (const auto& ride : assignedRides_) total += ride->getChargedFare();
    return total * kPayoutShare;
}

std::vector<std::shared_ptr<const Ride>> Driver::getAssignedRides() const {
    return {assignedRides_.begin(), assignedRides_.end()};
}

void Driver::getDriverInfo(std::ostream& out) const {
    out << "  " << name_ << " (" << driverID_ << ")  rating "
        << std::fixed << std::setprecision(2) << rating_
        << "  rides " << assignedRides_.size()
        << "  earnings " << money(earnings()) << "\n";
    for (const auto& ride : assignedRides_) {
        out << "      - " << ride->getRideID() << " " << ride->rideType()
            << ", " << toString(ride->getStatus()) << "\n";
    }
}
