# Format de config (3DS)

Fichier `config.json` placé sur la carte SD :

```json
{
  "nas_url": "https://nas.example.com",
  "nas_port": 443,
  "protocol": "https",
  "base_path": "/saves/3ds/",
  "username": "user",
  "password": "password"
}
```

- `nas_url` : URL ou IP du NAS.
- `nas_port` : port d’écoute (443 pour HTTPS, 22 pour SFTP, etc.).
- `protocol` : `https` ou `sftp`.
- `base_path` : chemin distant pour stocker les saves.
- `username` / `password` : identifiants de connexion.
