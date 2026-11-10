#include "Rider.h"

#include <ostream>
#include <stdexcept>
#include <utility>

Rider::Rider(std::string riderID, std::string name)
    : riderID_(std::move(riderID)), name_(std::move(name)) {}

void Rider::requestRide(const std::shared_ptr<Ride>& ride) {
    if (!ride) throw std::invalid_argument("cannot request an empty ride");
    requestedRides_.push_back(ride);
}

double Rider::totalSpent() const {
    double total = 0.0;
    for (const auto& ride : requestedRides_) total += ride->getChargedFare();
    return total;
}

void Rider::viewRides(std::ostream& out) const {
    out << "  " << name_ << " (" << riderID_ << ") - "
        << requestedRides_.size() << " rides, spent "
        << money(totalSpent()) << "\n";
    for (const auto& ride : requestedRides_) {
        ride->rideDetails(out);                  // polymorphic call
    }
}
