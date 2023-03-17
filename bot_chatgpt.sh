#!/bin/bash

if [[ -z $OPENAI_API_KEY ]]
then
	echo "Bot configure is not completed: OPENAI_API_KEY"
	exit
fi

function retrieve()
{
	body=$(cat << EOF
{
  "model": "gpt-3.5-turbo",
  "messages": [{"role": "user", "content": "$1"}],
  "temperature": 1.5,
  "presence_penalty": 1.5,
  "frequency_penalty": 1.5
}
EOF
)
	curl https://api.openai.com/v1/chat/completions \
-H "Content-Type: application/json" \
-H "Authorization: Bearer $OPENAI_API_KEY" \
-d "$body"
}

text=$(cat | tr '\n' ' ')
retrieve "$text" | python3 bot_parse_json.py
