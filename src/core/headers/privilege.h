#ifndef LO21_SPLENDOR_DUEL_PRIVILEGE_H
#define LO21_SPLENDOR_DUEL_PRIVILEGE_H
#include "Exception.h"
#include "card.h"
#include <iostream>
#include <stdexcept>
#include <vector>

class Privilege {
  private:
	const int id;
	static int instanceCount; // Current instance counter
	static const int maxInstances = 3;
	Privilege(const Privilege &) = delete;
	Privilege &operator=(const Privilege &) = delete;
	Privilege() : id(instanceCount) {}
	Privilege(int id) : id(id) {}

  public:
	json toJson() const {
		json j;
		j["id"] = id;
		return j;
	}

	static Privilege *getForJson(int id) {
		if (instanceCount < maxInstances) {
			++instanceCount;
			return new Privilege(id);
		}
		throw SplendorException("Number of privilege instances exceeded");
	}

	const int getId() const { return id; }
	static int getCount() { return instanceCount; }

	// Static method to obtain an instance of the class
	static const Privilege *GetInstance() {
		if (instanceCount < maxInstances) {
			++instanceCount;
			return new Privilege();
		}
		throw SplendorException("Number of privilege instances exceeded");
	}

	~Privilege() { instanceCount = 0; }

	static int getMaxInstance() { return maxInstances; }
};

inline std::ostream &operator<<(std::ostream &f, const Privilege &p) {
	f << "Id : " << p.getId();
	return f;
}
std::vector<const Privilege *> initPrivileges();

#endif // LO21_SPLENDOR_DUEL_PRIVILEGE_H
