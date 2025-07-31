//
//    AutoDGS: Show Marshaller or VDGS at default airports
//
//    Copyright (C) 2023, 2025 Holger Teutsch
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
//    USA
//

#include "simbrief.h"

#include <cassert>
#include <ctime>
#include <cstring>

#ifndef XPLM210
#error "need at least XPLM210"
#endif

#include "XPLMDataAccess.h"

#include "log_msg.h"

static bool drefs_loaded, sbh_unavail;

#define DEF_OFP_DR(f) static XPLMDataRef f ## _dr;
#define DEF_CDM_DR(f) static XPLMDataRef cdm_ ## f ## _dr;
DEF_OFP_DR(icao_airline);
DEF_OFP_DR(flight_number);
DEF_OFP_DR(aircraft_icao);
DEF_OFP_DR(destination);
DEF_OFP_DR(pax_count);
DEF_OFP_DR(est_out);
DEF_OFP_DR(est_off);
DEF_OFP_DR(est_on);
DEF_OFP_DR(est_in);
DEF_CDM_DR(tobt);
DEF_CDM_DR(tsat);
DEF_CDM_DR(runway);
DEF_CDM_DR(sid);

#undef DEF_OFP_DREF_DR
#undef DEF_CDM_DREF_DR

static XPLMDataRef seqno_dr, cdm_seqno_dr, stale_dr;
static int sbh_ofp_seqno, sbh_cdm_seqno, my_seqno;

// fetch byte data into a string
static void
FetchDref(std::string& str, XPLMDataRef dr)
{
    str.clear();
    auto n = XPLMGetDatab(dr, nullptr, 0, 0);
    if (n == 0)
        return;

    str.resize(n + 1);  // ensure we always have a trailing 0
    auto n1 = XPLMGetDatab(dr, (void *)str.data(), 0, n);
    assert(n == n1);
    // in case a 0-terminated string was returned make it a compatible std::string
    str.resize(strlen(str.c_str()));
}

#define FIND_OFP_DREF(f)  f ## _dr = XPLMFindDataRef("sbh/" #f)
#define FIND_CDM_DREF(f)  cdm_ ## f ## _dr = XPLMFindDataRef("sbh/cdm/" #f)

#define GET_OFP_DREF(f) FetchDref(ofp->f, f ## _dr)
#define GET_CDM_DREF(f) FetchDref(ofp->cdm_ ## f, cdm_ ## f ## _dr)
#define LOG_FIELD(f) LogMsg(" " #f ": '%s'", ofp->f.c_str())


std::unique_ptr<Ofp>
Ofp::LoadIfNewer([[maybe_unused]] int cur_seqno)
{
    if (sbh_unavail)
        return nullptr;

    if (!drefs_loaded) {
        stale_dr = XPLMFindDataRef("sbh/stale");
        if (stale_dr == nullptr) {
            sbh_unavail = true;
            LogMsg("simbrief_hub plugin is not loaded, bye!");
            return nullptr;
        }

        seqno_dr = XPLMFindDataRef("sbh/seqno");

        FIND_OFP_DREF(icao_airline);
        FIND_OFP_DREF(flight_number);
        FIND_OFP_DREF(aircraft_icao);
        FIND_OFP_DREF(destination);
        FIND_OFP_DREF(pax_count);
        FIND_OFP_DREF(est_out);
        FIND_OFP_DREF(est_off);
        FIND_OFP_DREF(est_on);
        FIND_OFP_DREF(est_in);

        FIND_CDM_DREF(tobt);
        if (cdm_tobt_dr) {      // for the transitional phase
            FIND_CDM_DREF(tsat);
            FIND_CDM_DREF(runway);
            FIND_CDM_DREF(sid);
        }
    }

    int ofp_seqno = XPLMGetDatai(seqno_dr);
    int cdm_seqno = XPLMGetDatai(cdm_seqno_dr);
    if (ofp_seqno == sbh_ofp_seqno && cdm_seqno == sbh_cdm_seqno)
       return nullptr;

    sbh_ofp_seqno = ofp_seqno;
    sbh_cdm_seqno = cdm_seqno;
    my_seqno++;

    int stale = XPLMGetDatai(stale_dr);
    if (stale)
        LogMsg("simbrief_hub data may be stale");

    auto ofp = std::make_unique<Ofp>();

    ofp->seqno = my_seqno;
    GET_OFP_DREF(icao_airline);
    GET_OFP_DREF(flight_number);
    GET_OFP_DREF(aircraft_icao);
    GET_OFP_DREF(destination);
    GET_OFP_DREF(pax_count);
    GET_OFP_DREF(est_out);
    GET_OFP_DREF(est_off);
    GET_OFP_DREF(est_on);
    GET_OFP_DREF(est_in);

    if (cdm_tobt_dr) {
        GET_CDM_DREF(tobt);
        GET_CDM_DREF(tsat);
        GET_CDM_DREF(runway);
        GET_CDM_DREF(sid);
    }

    LogMsg("From simbrief_hub: Seqno: %d, Cdm: %d", ofp_seqno, cdm_seqno);
    LOG_FIELD(icao_airline);
    LOG_FIELD(flight_number);
    LOG_FIELD(aircraft_icao);
    LOG_FIELD(destination);
    LOG_FIELD(pax_count);
    LOG_FIELD(est_out);
    LOG_FIELD(est_off);
    LOG_FIELD(est_on);
    LOG_FIELD(est_in);
    LOG_FIELD(cdm_tobt);
    LOG_FIELD(cdm_tsat);
    LOG_FIELD(cdm_runway);
    LOG_FIELD(cdm_sid);

    return ofp;
}

const std::string Ofp::GenDepartureStr() const {
    std::string str;
    str = icao_airline + flight_number + " " + aircraft_icao + " TO " + destination;

    time_t out_time = atol(est_out.c_str());
    time_t off_time = atol(est_off.c_str());

    bool have_cdm{false};
    if (cdm_tobt.empty()) {
        auto out_tm = *std::gmtime(&out_time);
        char out[20];
        strftime(out, sizeof(out), " OUT %H%M", &out_tm);
        str.append(out);
    } else {
        have_cdm = true;
        if (cdm_tsat != cdm_tobt) {
            str.append(" TOBT " + cdm_tobt);
        }

        if (!cdm_tsat.empty()) {
            str.append(" TSAT " + cdm_tsat);
        }
    }

    if (!have_cdm) {
        auto off_tm = *std::gmtime(&off_time);
        char off[20];
        strftime(off, sizeof(off), " OFF %H%M", &off_tm);
        str.append(off);
    }

    if (!cdm_runway.empty())
        str.append(" RWY " + cdm_runway);

    if (!cdm_sid.empty())
        str.append(" SID " + cdm_sid);

    return str;
}
