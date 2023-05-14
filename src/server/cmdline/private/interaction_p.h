#ifndef INTERACTION_P_H
#define INTERACTION_P_H

#include "cmdline/interaction.h"
#include "gts_global.h"

namespace gts::cmdline
{

class pipe_ope;

class GTS_DECL_HIDDEN interaction_private
{
public:
	explicit interaction_private(bool is_server);
	~interaction_private();

public:
	bool m_is_server;
	pipe_ope *m_ope = nullptr;
};

} //namespace gts::cmdline


#endif //INTERACTION_P_H
