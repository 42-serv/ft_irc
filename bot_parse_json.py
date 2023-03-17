#!/usr/bin/python3

import sys
import json

obj = json.loads(sys.stdin.read())

if "error" in obj:
	error = obj["error"]
	print(f'Error: [{error["type"]}]: {error["message"]}')
elif "choices" in obj:
	choices = obj["choices"]
	reply = str(choices[0]["message"]["content"])
	print(" ".join(reply.splitlines()))
else:
	print("Fatal: Unknown response format")
