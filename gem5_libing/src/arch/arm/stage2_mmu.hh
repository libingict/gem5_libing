/*
 * Copyright (c) 2012-2013 ARM Limited
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Thomas Grocutt
 */

#ifndef __ARCH_ARM_STAGE2_MMU_HH__
#define __ARCH_ARM_STAGE2_MMU_HH__

#include "arch/arm/faults.hh"
#include "arch/arm/tlb.hh"
#include "mem/request.hh"
#include "params/ArmStage2MMU.hh"
#include "sim/eventq.hh"

namespace ArmISA {

class Stage2MMU : public SimObject
{
  private:
    TLB *_stage1Tlb;
    /** The TLB that will cache the stage 2 look ups. */
    TLB *_stage2Tlb;

  public:
    /** This translation class is used to trigger the data fetch once a timing
        translation returns the translated physical address */
    class Stage2Translation : public BaseTLB::Translation
    {
      private:
        uint8_t   *data;
        int       numBytes;
        Request   req;
        Event     *event;
        Stage2MMU &parent;
        Addr      oVAddr;

      public:
        Fault fault;

        Stage2Translation(Stage2MMU &_parent, uint8_t *_data, Event *_event,
                          Addr _oVAddr);

        void
        markDelayed() {}

        void
        finish(const Fault &fault, RequestPtr req, ThreadContext *tc,
               BaseTLB::Mode mode);

        void setVirt(Addr vaddr, int size, Request::Flags flags, int masterId)
        {
            numBytes = size;
            req.setVirt(0, vaddr, size, flags, masterId, 0);
        }

        Fault translateTiming(ThreadContext *tc)
        {
            return (parent.stage2Tlb()->translateTiming(&req, tc, this, BaseTLB::Read));
        }
    };

    typedef ArmStage2MMUParams Params;
    Stage2MMU(const Params *p);

    Fault readDataUntimed(ThreadContext *tc, Addr oVAddr, Addr descAddr,
        uint8_t *data, int numBytes, Request::Flags flags, int masterId,
        bool isFunctional);
    Fault readDataTimed(ThreadContext *tc, Addr descAddr,
        Stage2Translation *translation, int numBytes, Request::Flags flags,
        int masterId);

    TLB* stage1Tlb() const { return _stage1Tlb; }
    TLB* stage2Tlb() const { return _stage2Tlb; }
};



} // namespace ArmISA

#endif //__ARCH_ARM_STAGE2_MMU_HH__

