#ifndef RIDE_H
#define RIDE_H

#include <iosfwd>
#include <string>

enum class RideStatus { Requested, Assigned, Completed, Cancelled };

std::string toString(RideStatus status);
std::string money(double amount);  // 16.6 -> "$16.60"

// ---------------------------------------------------------------------------
// Base class. Every field is private: subclasses and outside code can only
// read them through getters and change state through the lifecycle methods.
// ---------------------------------------------------------------------------
class Ride {
public:
    Ride(std::string rideID, std::string pickup, std::string dropoff,
         double distanceMiles);
    virtual ~Ride() = default;

    // Polymorphic interface: dispatched at run time through the vtable.
    virtual double fare() const;                     // $2.00 + $1.25/mi
    virtual std::string rideType() const;
    virtual void rideDetails(std::ostream& out) const;

    // Lifecycle: Requested -> Assigned -> Completed (or Cancelled)
    void assignTo(const std::string& driverID);
    void complete();                                 // locks in the fare
    void cancel();

    const std::string& getRideID() const { return rideID_; }
    const std::string& getPickup() const { return pickupLocation_; }
    const std::string& getDropoff() const { return dropoffLocation_; }
    const std::string& getDriverID() const { return driverID_; }
    double getDistance() const { return distance_; }
    double getChargedFare() const { return fare_; }
    RideStatus getStatus() const { return status_; }

protected:
    static double roundCents(double amount);

private:
    std::string rideID_;
    std::string pickupLocation_;
    std::string dropoffLocation_;
    double distance_;
    double fare_ = 0.0;                   // amount charged, set by complete()
    RideStatus status_ = RideStatus::Requested;
    std::string driverID_;
};

// Everyday car: $2.50 booking fee + $1.50/mi, $7.00 minimum.
class StandardRide : public Ride {
public:
    using Ride::Ride;                     // inherit the constructor
    double fare() const override;
    std::string rideType() const override;
};

// Luxury vehicle: $5.00 + $3.25/mi, $15.00 minimum.
class PremiumRide : public Ride {
public:
    PremiumRide(std::string rideID, std::string pickup, std::string dropoff,
                double distanceMiles, std::string vehicleClass);
    double fare() const override;
    std::string rideType() const override;
    void rideDetails(std::ostream& out) const override;

private:
    std::string vehicleClass_;
};

// Pooled ride: a StandardRide discounted 15% per extra rider (max 45%).
// Inherits from StandardRide, so the hierarchy is three levels deep.
class SharedRide : public StandardRide {
public:
    SharedRide(std::string rideID, std::string pickup, std::string dropoff,
               double distanceMiles, int riders);
    double fare() const override;
    std::string rideType() const override;
    void rideDetails(std::ostream& out) const override;
    double discount() const;

private:
    int riders_;
};

#endif
