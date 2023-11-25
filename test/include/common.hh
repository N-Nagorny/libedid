// Copyright 2023 N-Nagorny
#pragma once

#include "edid/base_block.hh"
#include "edid/cta861_block.hh"

Edid::BaseBlock make_edid_base();
Edid::Cta861Block make_cta861_ext();
