import "libc";

// FIXME: Only works on macOS right now; arrow needs primitives for dealing with
//        platform differences. Only works on SAFARI (macOS 10.12). Needs
//        a whole lot of IFs.

/// A Duration represents the elapsed time between two points
/// as an int64 nanosecond count.
type Duration = int64;

implement Duration {
  /// Returns the duration as a floating point number of seconds.
  def Seconds(self): float64 {
    let s = int64(self) / 1_000_000_000;
    let ns = int64(self) % 1_000_000_000;

    return float64(s) + float64(ns) * 1e-9;
  }
}

/// A point in time.
struct Time {
  // Whole seconds (valid values are >= 0)
  _sec: int64;

  // Nanoseconds (valid values are [0, 999999999])
  _nsec: int64;
}

implement Time {
  /// Returns the duration between two points in time.
  def Sub(self, u: Time): Duration {
    let d: int64 = 0;
    d += (self._sec - u._sec) * 1_000_000_000;
    d += self._nsec - u._nsec;

    // FIXME: Handle over/under-flow

    return Duration(d);
  }
}

/// Returns a `Time` representing the current point in time.
def Now(): Time {
  // FIXME: Handle failure
  let ts: libc.timespec;
  // FIXME: Macros are not exported by libc
  libc.clock_gettime(libc._CLOCK_REALTIME, &ts);

  // FIXME: Cleanup when we have record literals
  let t: Time;
  t._sec = ts.tv_sec;
  t._nsec = ts.tv_nsec;

  return t;
}

/// Returns a `Time` representing the current value of the clock.
def Monotonic(): Time {
  // FIXME: Handle failure
  let ts: libc.timespec;
  // FIXME: Macros are not exported by libc
  libc.clock_gettime(libc._CLOCK_MONOTONIC_RAW, &ts);

  // FIXME: Cleanup when we have record literals
  let t: Time;
  t._sec = ts.tv_sec;
  t._nsec = ts.tv_nsec;

  return t;
}