#define HAITI                                                                  \
  "haiti --no-color --hashcat-only -e %s | grep -oP '\\[HC: \\K.*' | sed "     \
  "'s/.$//'"

#define HASHCAT                                                                \
  "hashcat -a 3 -m %s hashes_to_process/%s ?a --potfile-disable --quiet -o "   \
  "hashes_cracked/%s"
