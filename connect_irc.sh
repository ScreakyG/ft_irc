#!/bin/bash

# Paramètres par défaut
PORT=4242
PASSWORD="lol"
HOST="localhost"

# Lecture des arguments
while getopts "n:p:h:w:" opt; do
    case $opt in
        n) NICK="$OPTARG";;
        p) PORT="$OPTARG";;  # Cette option permet de changer le port
        h) HOST="$OPTARG";;
        w) PASSWORD="$OPTARG";;
        \?) echo "Usage: $0 [-n nickname] [-p port] [-h host] [-w password]" >&2; exit 1;;
    esac
done

# Vérification du nickname
if [ -z "$NICK" ]; then
    echo "Nickname requis (-n nickname)"
    exit 1
fi

# Vérification que le port est un nombre valide
if ! [[ "$PORT" =~ ^[0-9]+$ ]]; then
    echo "Le port doit être un nombre"
    exit 1
fi

# Fichier temporaire
TMPFILE=$(mktemp)
trap "rm -f $TMPFILE" EXIT

# Commandes d'identification
cat > $TMPFILE << EOF
PASS ${PASSWORD}
NICK ${NICK}
USER ${NICK} * *:${NICK} Real Name
EOF

# Affichage des informations de connexion
echo "Connexion à ${HOST}:${PORT} avec le nickname ${NICK}"

# Connexion avec maintien de la session
(cat $TMPFILE; cat) | nc -C ${HOST} ${PORT}
