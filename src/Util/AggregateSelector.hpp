/*
 * AggregateSelector.hpp
 *
 *  Created on: 21.08.2017
 *      Author: tk4
 */

#pragma once

#include <Util/TestConfiguration.hpp>

struct AggregateSelector {
    bool operator()(
            AggregateConfiguration::Sum) {
        return true;
    }
    bool operator()(
            AggregateConfiguration::Min) {
        return false;
    }
    bool operator()(
            AggregateConfiguration::Max) {
        return false;
    }
    bool operator()(
            AggregateConfiguration::Avg) {
        return false;
    }
};
