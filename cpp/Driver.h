#ifndef DRIVER_H
#define DRIVER_H

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "Ride.h"

class Driver {
public:
    Driver(std::string driverID, std::string name, double rating,
           int ratingsCount);

    void addRide(const std::shared_ptr<Ride>& ride);
    void rate(int stars);                        // 1-5, updates the average
    void getDriverInfo(std::ostream& out) const;

    const std::string& getDriverID() const { return driverID_; }
    const std::string& getName() const { return name_; }
    double getRating() const { return rating_; }
    std::size_t rideCount() const { return assignedRides_.size(); }
    std::size_t activeRideCount() const;         // assigned, not finished
    double earnings() const;                     // driver's share of fares

    // Read-only snapshot: callers get a copy, never the private list itself.
    std::vector<std::shared_ptr<const Ride>> getAssignedRides() const;

private:
    static constexpr double kPayoutShare = 0.80;  // platform keeps 20%

    std::string driverID_;
    std::string name_;
    double rating_;
    int ratingsCount_;
    std::vector<std::shared_ptr<Ride>> assignedRides_;
};

#endif
