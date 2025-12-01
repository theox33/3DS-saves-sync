# Protocole de synchronisation

## Upload (HTTPS)

- Endpoint : `POST /upload`
- Content-Type : `multipart/form-data`
- Champs :
  - `file`: archive de sauvegarde (zip/tar)
  - `game`: (optionnel) identifiant du jeu
- Réponse :

```json
{ "status": "ok", "filename": "game_xxx.zip" }
```

## Liste

- Endpoint : `GET /list`
- Réponse :

```json
["game1_save1.zip", "game2_save2.zip"]
```

## Download

- Endpoint : `GET /download/{filename}`
- Réponse : fichier binaire (archive de sauvegarde).
