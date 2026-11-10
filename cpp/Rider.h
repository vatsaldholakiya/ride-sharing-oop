#ifndef RIDER_H
#define RIDER_H

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "Ride.h"

class Rider {
public:
    Rider(std::string riderID, std::string name);

    void requestRide(const std::shared_ptr<Ride>& ride);
    void viewRides(std::ostream& out) const;     // ride history
    double totalSpent() const;                   // completed rides only

    const std::string& getRiderID() const { return riderID_; }
    const std::string& getName() const { return name_; }

private:
    std::string riderID_;
    std::string name_;
    std::vector<std::shared_ptr<Ride>> requestedRides_;
};

#endif
