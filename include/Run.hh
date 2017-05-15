#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "G4Run.hh"
#include "G4Event.hh"

#include "G4THitsMap.hh"

//---------------------------------------------------------------------
/// Run class
///
/// Example implementation for multi-functional-detector and primitive scorer.
/// This Run class has collections which accumulate
/// a event information into a run information.
//---------------------------------------------------------------------

class Run : public G4Run
{
#pragma region Data
    private: std::vector<std::string>          _CollName;
    private: std::vector<int>                  _CollID;
    private: std::vector<G4THitsMap<double>*>  _runMap;
#pragma endregion

#pragma region Ctor/Dtor/ops
    public: Run();
    public: Run(const std::vector<std::string> mfdName);
    public: virtual ~Run();
#pragma endregion

#pragma region Interfaces
    public:  virtual void RecordEvent(const G4Event*) override;
#pragma endregion

#pragma region Observers
    // Access methods for scoring information.
    // - Number of HitsMap for this RUN.
    //   This is equal to number of collections.
    public: size_t GetNumberOfHitsMap() const
    {
        return _runMap.size();
    }

    // - Get HitsMap of this RUN.
    //   by sequential number, by multifucntional name and collection name,
    //   and by collection name with full path.
    public: G4THitsMap<double>* GetHitsMap(size_t i) const
    {
        return _runMap[i];
    }

    public: G4THitsMap<double>* GetHitsMap(const std::string& detName,
                                           const std::string& colName) const;

    public: G4THitsMap<double>* GetHitsMap(const std::string& fullName) const;

    void ConstructMFD(const std::vector<std::string>&);

    virtual void Merge(const G4Run*) override;
#pragma endregion
};

//==========================================================================
//          Generic Functions to help with merge
//==========================================================================
template <typename T> inline void
copy(std::vector<T>& main, const std::vector<T>& data)
{
    for(auto i = main.size(); i != data.size(); ++i)
    {
        main.push_back(data.at(i));
    }
}

template <typename T> inline size_t
copy(std::vector<T*>& main, const std::vector<T*>& data)
{
    auto size_diff = data.size() - main.size();
    for(auto i = main.size(); i != data.size(); ++i)
    {
        main.push_back(new T(*data.at(i)));
    }
    return size_diff;
}

template <typename T> inline std::ostream&
print(const std::vector<T>& data, std::ostream& os)
{
    os << std::endl;
    for(size_t i = 0; i != data.size(); ++i)
    {
        os << "\t\t" << i << " \t\t " << data.at(i) << std::endl;
    }
    os << std::endl;
    return os;
}

template <typename T> inline std::ostream&
print(const std::vector<T>& data)
{
    return print(data, std::cout);
}
