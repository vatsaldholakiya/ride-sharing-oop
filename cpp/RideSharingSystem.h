#ifndef RIDE_SHARING_SYSTEM_H
#define RIDE_SHARING_SYSTEM_H

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "Driver.h"
#include "Ride.h"
#include "Rider.h"

// Dispatcher: owns drivers and riders and keeps every ride, of every type,
// in one list of base-class pointers.
class RideSharingSystem {
public:
    Driver& addDriver(const std::string& id, const std::string& name,
                      double rating, int ratingsCount);
    Rider& addRider(const std::string& id, const std::string& name);

    // Records the request and assigns the least-busy, best-rated driver.
    Driver& requestRide(Rider& rider, const std::shared_ptr<Ride>& ride);
    void completeRide(const std::string& rideID, int stars);
    void cancelRide(const std::string& rideID);

    void showAllRides(std::ostream& out) const;
    void printFareReport(std::ostream& out) const;

private:
    Driver& pickDriver();
    Driver& driverFor(const Ride& ride);
    std::shared_ptr<Ride> findRide(const std::string& rideID);

    std::vector<std::unique_ptr<Driver>> drivers_;
    std::vector<std::unique_ptr<Rider>> riders_;
    std::vector<std::shared_ptr<Ride>> rides_;   // mixed ride types
};

#endif
