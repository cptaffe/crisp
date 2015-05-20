
#ifndef CRISP_STATE_H_
#define CRISP_STATE_H_

namespace crisp {

class StateInterface {
public:
	virtual ~StateInterface() {}
	// run state and get next state.
	virtual StateInterface *Next() = 0;
};

} // namespace crisp

#endif // CRISP_STATE_H_
