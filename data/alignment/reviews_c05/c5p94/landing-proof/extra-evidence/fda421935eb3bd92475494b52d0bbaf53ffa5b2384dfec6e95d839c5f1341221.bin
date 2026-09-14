# AllFixtureTests.cmake — the battery tells the truth about what it ran.
#
# BUILD-7 (SQA 2026-08-22): 37 of the 73 registered suites need data that
# git does not track (build/, library/, data/extracted/). From a clean
# checkout those suites either aborted with an uncaught exception or —
# worse — printed [PASS] over zero rows, so "72/72" did not mean what it
# said. A suite that silently does not run is worse than one that says
# why.
#
# This file overrides add_test(). Every argument that names a path under
# the source tree is checked at CONFIGURE time:
#
#   * all present  -> the suite is registered unchanged
#   * any absent   -> the suite is registered as a ctest SKIP whose one
#                     line of output names the exact missing path and
#                     how to produce it
#
# A skipped suite is reported by ctest as "***Skipped" and listed under
# "The following tests did not run" — it is never counted as a pass.
# Suites that touch an untracked root also carry the label "fixture", so
#
#   ctest -LE fixture     is the clean-checkout / CI gate
#   ctest                 is the press gate (and the press refuses to
#                         ship if anything skipped — package_macos.sh)
#
# docs/FIXTURES.md is the inventory: every untracked path the battery
# needs, which suites need it, and the tool that produces it (or an
# explicit "no generator in this repo", because guessing is worse).

# Source-tree roots that .gitignore keeps out of the repository.
set(ALL_UNTRACKED_ROOTS build library data/extracted data/das data/teaching
    CACHE INTERNAL "source-tree roots git does not track")

# How a developer gets the missing thing back. Truthful and coarse: the
# per-file generator table lives in docs/FIXTURES.md, which the
# fixture_gate suite keeps in step with this list.
function(_all_fixture_hint _out _rel)
  set(_h "not present in this checkout — see docs/FIXTURES.md")
  if(_rel MATCHES "^build(/|$)")
    set(_h "a generated fixture: docs/FIXTURES.md names the tool that produces it, or says none exists in this repo")
  elseif(_rel MATCHES "^library(/|$)")
    set(_h "the text library is not in git: install a collection under library/ — docs/FIXTURES.md")
  elseif(_rel MATCHES "^data/extracted(/|$)")
    set(_h "a derived extract: docs/FIXTURES.md names the tool that produces it, or says none exists in this repo")
  endif()
  set(${_out} "${_h}" PARENT_SCOPE)
endfunction()

# Overriding a built-in leaves the original reachable as _add_test().
function(add_test)
  set(_argv ${ARGV})
  list(FIND _argv "NAME" _name_at)
  if(_name_at LESS 0)
    # the old add_test(<name> <command>) form — nothing to inspect
    _add_test(${ARGV})
    return()
  endif()
  math(EXPR _name_at "${_name_at} + 1")
  list(GET _argv ${_name_at} _name)

  string(LENGTH "${CMAKE_SOURCE_DIR}/" _src_len)
  set(_missing "")
  set(_hints "")
  set(_needs_fixture FALSE)
  foreach(_a IN LISTS _argv)
    string(FIND "${_a}" "${CMAKE_SOURCE_DIR}/" _at)
    if(NOT _at EQUAL 0)
      continue()
    endif()
    string(SUBSTRING "${_a}" ${_src_len} -1 _rel)
    if(_rel STREQUAL "")
      continue()
    endif()
    foreach(_root IN LISTS ALL_UNTRACKED_ROOTS)
      if(_rel STREQUAL "${_root}" OR _rel MATCHES "^${_root}/")
        set(_needs_fixture TRUE)
      endif()
    endforeach()
    if(NOT EXISTS "${_a}")
      list(APPEND _missing "${_rel}")
      _all_fixture_hint(_hint "${_rel}")
      list(APPEND _hints "${_hint}")
    endif()
  endforeach()

  if(_missing)
    list(REMOVE_DUPLICATES _missing)
    list(REMOVE_DUPLICATES _hints)
    string(REPLACE ";" ", " _missing_text "${_missing}")
    string(REPLACE ";" " · " _hint_text "${_hints}")
    _add_test(NAME "${_name}" COMMAND "${CMAKE_COMMAND}" -E echo
              "[SKIP] ${_name}: fixture absent: ${_missing_text} — ${_hint_text}")
    set_tests_properties("${_name}" PROPERTIES
      SKIP_REGULAR_EXPRESSION "\\[SKIP\\] "
      LABELS "fixture;fixture-missing")
    set_property(GLOBAL APPEND PROPERTY ALL_SKIPPED_SUITES "${_name}")
  else()
    _add_test(${ARGV})
    set_property(GLOBAL APPEND PROPERTY ALL_RUNNABLE_SUITES "${_name}")
    if(_needs_fixture)
      set_tests_properties("${_name}" PROPERTIES LABELS "fixture")
    endif()
  endif()
endfunction()

# Called once, after every add_subdirectory(), so the configure log says
# out loud how much of the battery this checkout can actually run.
function(all_report_fixture_status)
  get_property(_skipped GLOBAL PROPERTY ALL_SKIPPED_SUITES)
  get_property(_runnable GLOBAL PROPERTY ALL_RUNNABLE_SUITES)
  list(LENGTH _skipped _n_skipped)
  list(LENGTH _runnable _n_runnable)
  math(EXPR _n_total "${_n_skipped} + ${_n_runnable}")
  if(_n_skipped GREATER 0)
    string(REPLACE ";" ", " _skipped_text "${_skipped}")
    message(STATUS
      "ALL battery: ${_n_runnable}/${_n_total} suites can run here; "
      "${_n_skipped} will SKIP for want of untracked fixtures — ${_skipped_text}")
    message(STATUS
      "ALL battery: 'ctest -LE fixture' is the clean-checkout gate; the "
      "press requires all ${_n_total}. See docs/FIXTURES.md")
  else()
    message(STATUS
      "ALL battery: all ${_n_total} suites have their fixtures present")
  endif()
endfunction()
