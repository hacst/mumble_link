#!/usr/bin/env python
# -*- coding: utf-8

# Copyright (C) 2013 Stefan Hacker <dd0t@users.sourceforge.net>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:

# - Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
# - Neither the name of the Mumble Developers nor the names of its
#   contributors may be used to endorse or promote products derived from this
#   software without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# `AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import mumble_link
import sys

if __name__ == "__main__":
	if len(sys.argv) != 2:
		print>>sys.stderr, "Usage: %s [stepfile]" % sys.argv[0]
		sys.exit(1)
	
	try:
		cfg = __import__(sys.argv[1])
		
		name = cfg.name
		description = cfg.description
		
		steps = cfg.steps
	except (ImportError, AttributeError), e:
		print>>sys.stderr, "Failed to get step list: " + str(e)
		sys.exit(1)
		
	mumble_link.setup(name, description)
	
	cnt = 0
	for step in steps:
		for what, parameters in step:
			cmd = "set_%s" % what
			print "%.3d %s(*%s)" % (cnt, cmd, repr(parameters))

		raw_input("Press enter to execute step")
		
		for what, parameters in step:
			cmd = "set_%s" % what
			getattr(mumble_link, cmd)(*parameters)
		cnt += 1
		
	raw_input("Steps completed, press enter to quit")
