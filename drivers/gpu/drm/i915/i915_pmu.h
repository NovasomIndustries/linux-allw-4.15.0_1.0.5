/*
 * Copyright © 2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */
#ifndef __I915_PMU_H__
#define __I915_PMU_H__

enum {
	__I915_SAMPLE_FREQ_ACT = 0,
	__I915_SAMPLE_FREQ_REQ,
	__I915_NUM_PMU_SAMPLERS
};

/**
 * How many different events we track in the global PMU mask.
 *
 * It is also used to know to needed number of event reference counters.
 */
#define I915_PMU_MASK_BITS \
	((1 << I915_PMU_SAMPLE_BITS) + \
	 (I915_PMU_LAST + 1 - __I915_PMU_OTHER(0)))

struct i915_pmu_sample {
	u64 cur;
};

struct i915_pmu {
	/**
	 * @node: List node for CPU hotplug handling.
	 */
	struct hlist_node node;
	/**
	 * @base: PMU base.
	 */
	struct pmu base;
	/**
	 * @lock: Lock protecting enable mask and ref count handling.
	 */
	spinlock_t lock;
	/**
	 * @timer: Timer for internal i915 PMU sampling.
	 */
	struct hrtimer timer;
	/**
	 * @enable: Bitmask of all currently enabled events.
	 *
	 * Bits are derived from uAPI event numbers in a way that low 16 bits
	 * correspond to engine event _sample_ _type_ (I915_SAMPLE_QUEUED is
	 * bit 0), and higher bits correspond to other events (for instance
	 * I915_PMU_ACTUAL_FREQUENCY is bit 16 etc).
	 *
	 * In other words, low 16 bits are not per engine but per engine
	 * sampler type, while the upper bits are directly mapped to other
	 * event types.
	 */
	u64 enable;
	/**
	 * @enable_count: Reference counts for the enabled events.
	 *
	 * Array indices are mapped in the same way as bits in the @enable field
	 * and they are used to control sampling on/off when multiple clients
	 * are using the PMU API.
	 */
	unsigned int enable_count[I915_PMU_MASK_BITS];
	/**
	 * @timer_enabled: Should the internal sampling timer be running.
	 */
	bool timer_enabled;
	/**
	 * @sample: Current and previous (raw) counters for sampling events.
	 *
	 * These counters are updated from the i915 PMU sampling timer.
	 *
	 * Only global counters are held here, while the per-engine ones are in
	 * struct intel_engine_cs.
	 */
	struct i915_pmu_sample sample[__I915_NUM_PMU_SAMPLERS];
};

#ifdef CONFIG_PERF_EVENTS
void i915_pmu_register(struct drm_i915_private *i915);
void i915_pmu_unregister(struct drm_i915_private *i915);
void i915_pmu_gt_parked(struct drm_i915_private *i915);
void i915_pmu_gt_unparked(struct drm_i915_private *i915);
#else
static inline void i915_pmu_register(struct drm_i915_private *i915) {}
static inline void i915_pmu_unregister(struct drm_i915_private *i915) {}
static inline void i915_pmu_gt_parked(struct drm_i915_private *i915) {}
static inline void i915_pmu_gt_unparked(struct drm_i915_private *i915) {}
#endif

#endif
