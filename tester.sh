#!/usr/bin/env bash
# test_signaux_plus.sh
# Test amélioré pour client/serveur (minitalk-like) avec mesure de temps et tests séquentiels

set -u
SERVER_BIN="./server"
CLIENT_BIN="./client"
TIMEOUT_CLIENT=5

# === LISTE DE TESTS ===
TEST_STRINGS=(
  "Bonjour"
  "Test 123"
  "Hello, world!"
  ""                                # chaîne vide
  "abc"
  "OK"
  "Avec espaces et !@#"
  "Unicode 👋🌍😀🔥"
  "Long_$(head /dev/urandom | tr -dc 'a-zA-Z0-9' | head -c 3000)"
  "Repeat_XXXXX"
  "!@#%&*(){}[]?"
  "$(head /dev/urandom | tr -dc 'a-z0-9' | head -c 10000)"
)

LOG="$(mktemp /tmp/serveur_log.XXXXXX)"

# === Vérifications ===
if [[ ! -x "$SERVER_BIN" ]]; then
  echo "Erreur : serveur introuvable ou non exécutable : $SERVER_BIN"
  exit 2
fi
if [[ ! -x "$CLIENT_BIN" ]]; then
  echo "Erreur : client introuvable ou non exécutable : $CLIENT_BIN"
  exit 2
fi

cleanup() {
  echo
  echo "=== Nettoyage ==="
  if [[ -n "${SERVER_PID-}" ]] && kill -0 "$SERVER_PID" 2>/dev/null; then
    echo "Arrêt du serveur (pid $SERVER_PID)..."
    kill "$SERVER_PID" 2>/dev/null || true
    sleep 0.2
    kill -9 "$SERVER_PID" 2>/dev/null || true
  fi
  echo "Log du serveur enregistré dans : $LOG"
}
trap cleanup EXIT

# === Lancer le serveur ===
echo "Lancement du serveur : $SERVER_BIN"
"$SERVER_BIN" > "$LOG" 2>&1 &
SERVER_PID=$!
sleep 0.2

if ! kill -0 "$SERVER_PID" 2>/dev/null; then
  echo "Le serveur n'a pas démarré correctement. Voir $LOG"
  exit 3
fi

echo "Serveur lancé (pid $SERVER_PID). Log : $LOG"
echo

# === Fonction d'exécution client ===
run_client() {
  local pid="$1"
  local str="$2"

  if command -v timeout >/dev/null 2>&1; then
    timeout "${TIMEOUT_CLIENT}" "$CLIENT_BIN" "$pid" "$str"
    return $?
  else
    "$CLIENT_BIN" "$pid" "$str"
    return $?
  fi
}

success_count=0
fail_count=0
test_no=0

# === Récupérer la sortie du log depuis une position donnée ===
get_new_log() {
  local before="$1"
  if [[ ! -f "$LOG" ]]; then
    echo ""
    return
  fi
  local size_now
  size_now=$(stat -c%s "$LOG" 2>/dev/null || echo 0)
  if (( size_now <= before )); then
    echo ""
  else
    tail -c +"$((before + 1))" "$LOG" 2>/dev/null || cat "$LOG"
  fi
}

# === Tests individuels ===
for s in "${TEST_STRINGS[@]}"; do
  test_no=$((test_no+1))
  echo "=== Test #$test_no ==="
  echo "Chaîne envoyée: [${s:0:80}] (longueur: ${#s})"

  prev_size=0
  [[ -f "$LOG" ]] && prev_size=$(stat -c%s "$LOG" 2>/dev/null || echo 0)

  start_time=$(date +%s%3N)  # millisecondes
  run_client "$SERVER_PID" "$s"
  client_rc=$?
  end_time=$(date +%s%3N)
  duration_ms=$((end_time - start_time))

  ratio=0
  if (( ${#s} > 0 )); then
    ratio=$(awk "BEGIN {printf \"%.2f\", $duration_ms / (${#s}/100.0)}")
  fi

  if [[ $client_rc -ne 0 ]]; then
    echo "Client a quitté avec un code non nul: $client_rc"
    echo ">>> Voir $LOG"
    fail_count=$((fail_count+1))
    echo
    continue
  fi

  sleep 0.5
  received="$(get_new_log "$prev_size" | tr -d '\r' | sed -n 's/^[[:space:]]*//;s/[[:space:]]*$//p')"

  if [[ -z "$s" ]]; then
    if [[ -n "$received" ]]; then
      echo "OK — Réponse détectée (chaîne vide)"
      success_count=$((success_count+1))
    else
      echo "ÉCHEC — aucune sortie pour la chaîne vide."
      fail_count=$((fail_count+1))
    fi
  else
    if printf "%s" "$received" | grep -F -- "$s" >/dev/null 2>&1; then
      echo "✅ OK — chaîne trouvée dans le log."
      echo "⏱️ Temps total: ${duration_ms} ms (≈ ${ratio} ms / 100 caractères)"
      success_count=$((success_count+1))
    else
      echo "❌ ÉCHEC — la chaîne n’a pas été trouvée."
      echo "----- Sortie récente -----"
      printf "%s\n" "$received" | head -n 10
      echo "--------------------------"
      fail_count=$((fail_count+1))
    fi
  fi
  echo
done

# === Test multi-clients séquentiels ===
echo "=== Test séquentiel multi-clients ==="
CLIENT_MSGS=(
  "Client1_$(head /dev/urandom | tr -dc 'A-Z' | head -c 6)"
  "Client2_$(head /dev/urandom | tr -dc 'A-Z' | head -c 6)"
  "Client3_$(head /dev/urandom | tr -dc 'A-Z' | head -c 6)"
  "Client4_$(head /dev/urandom | tr -dc 'A-Z' | head -c 6)"
)

ok_all=true
for msg in "${CLIENT_MSGS[@]}"; do
  echo "→ Envoi séquentiel du message : $msg"
  prev_size=$(stat -c%s "$LOG" 2>/dev/null || echo 0)
  start_time=$(date +%s%3N)
  run_client "$SERVER_PID" "$msg"
  end_time=$(date +%s%3N)
  sleep 0.5
  received="$(get_new_log "$prev_size")"
  duration_ms=$((end_time - start_time))
  if printf "%s" "$received" | grep -q "$msg"; then
    echo "✅ Reçu. Temps : ${duration_ms} ms"
  else
    echo "❌ Non détecté dans le log."
    ok_all=false
  fi
done

if $ok_all; then
  echo "OK — le serveur a bien traité les messages de plusieurs clients successifs."
  success_count=$((success_count+1))
else
  echo "ÉCHEC — au moins un message séquentiel manquant."
  fail_count=$((fail_count+1))
fi

# === Résumé final ===
echo
echo "=== Résumé ==="
echo "Tests réussis : $success_count"
echo "Tests échoués : $fail_count"
echo "Log complet : $LOG"

exit $((fail_count>0))
