source = 'Source engine: %s'

name = source % 'nope'
description = "Just testing stuff"

steps = [
[	
	['version', [0]]
],
[
	['version', [2]],
	['name', [source % 'tf']],
	['context', ['[0:1]\x00']],
	['identity', ['universe:1;account_type:1;id:24381725;instance:1;team:0']]
],
[
	['identity', ['universe:1;account_type:1;id:24381725;instance:1;team:2']]
],
[
	['context', ['[A-1:915568644(3282)]\x00']]
],
[
	['context', ['invalid']]
],
[
	['context', ['[A-1:915568644(3282)]\x00']]
],
[
	['identity', ['invalid']]
],
[
	['identity', ['universe:1;account_type:1;id:24381725;instance:1;team:3']]
],
[
	['version', [0]]
],
[
	['version', [2]],
	['name', [source % 'cstrike']],
	['context', ['[A-1:2807761920(3281)]\x00']],
	['identity', ['universe:1;account_type:1;id:24381725;instance:1;team:0']]
],
[
	['identity', ['universe:1;account_type:1;id:24381725;instance:1;team:2']],
],
[
	['identity', ['universe:1;account_type:1;id:24381725;instance:1;team:3']]
],
[
	['version', [0]]
],
[
	['version', [2]],
	['name', [source % 'dod']],
	['context', ['[A-1:2807761920(3281)]\x00']],
	['identity', ['universe:1;account_type:1;id:48239812;instance:1;team:2']]
],
[
	['context', ['[A-1:2227761220(3281)]\x00']],
	['identity', ['universe:1;account_type:1;id:48239812;instance:1;team:2']]
]
]
