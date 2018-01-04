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
        return true;
    }
    bool operator()(
            AggregateConfiguration::Max) {
        return true;
    }
    bool operator()(
            AggregateConfiguration::Avg) {
        return true;
    }
};
