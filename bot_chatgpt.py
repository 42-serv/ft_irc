#!/usr/bin/python3

import sys
import os
import json
import http.client

if not "OPEN_API_KEY" in os.environ:
	print("Bot configure is not completed: OPEN_API_KEY")
	exit()

api_key = os.environ["OPEN_API_KEY"]

text = " ".join(sys.stdin.read().splitlines())

data = f"""{{
    "model": "gpt-3.5-turbo",
    "messages": [{{"role": "user", "content": "{text}"}}]
}}"""

headers = {
	"Content-type": "application/json",
	"Authorization": f"Bearer {api_key}"
}

conn = http.client.HTTPSConnection("api.openai.com")

conn.request("POST", "/v1/chat/completions", data, headers)

response = conn.getresponse()

obj = json.loads(response.read())

if "error" in obj:
	error = obj["error"]
	print(f'Error: [{error["type"]}]: {error["message"]}')
elif "choices" in obj:
	choices = obj["choices"]
	reply = str(choices[0]["message"]["content"])
	print(" ".join(reply.splitlines()))
else:
	print("Unknown response format")
