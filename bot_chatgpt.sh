#!/bin/bash

if [[ -z $OPENAI_API_KEY ]]
then
	echo "Bot configure is not completed: OPENAI_API_KEY"
	exit
fi

function sanitize_json()
{
	python3 -c 'import json, sys; print(json.dumps(sys.stdin.read()))'
}

function subst_newline()
{
	tr '\n' ' '
}

function retrieve()
{
	body=$(cat << EOF
{
  "model": "gpt-3.5-turbo",
  "messages": [{"role": "user", "content": $1}]
}
EOF
)
	curl https://api.openai.com/v1/chat/completions \
		-H "Content-Type: application/json" \
		-H "Authorization: Bearer $OPENAI_API_KEY" \
		-d "$body"
}

query=$(sanitize_json | subst_newline)
retrieve "$query" | python3 bot_parse_json.py | subst_newline
