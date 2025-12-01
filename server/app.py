from fastapi import FastAPI, UploadFile, File, HTTPException
from fastapi.responses import FileResponse
from pathlib import Path
from typing import List

app = FastAPI(title="3DS Save Sync API")

STORAGE_DIR = Path(__file__).parent / "storage"
STORAGE_DIR.mkdir(exist_ok=True)


@app.get("/")
def root():
    return {"status": "ok", "message": "3DS Save Sync API"}


@app.post("/upload")
async def upload_save(file: UploadFile = File(...), game: str | None = None):
    if not file.filename:
        raise HTTPException(status_code=400, detail="No filename provided")

    filename = file.filename
    if game:
        filename = f"{game}_{file.filename}"

    dest = STORAGE_DIR / filename

    with dest.open("wb") as f:
        content = await file.read()
        f.write(content)

    return {"status": "ok", "filename": filename}


@app.get("/list")
def list_saves() -> List[str]:
    files = [p.name for p in STORAGE_DIR.iterdir() if p.is_file()]
    return files


@app.get("/download/{filename}")
def download_save(filename: str):
    file_path = STORAGE_DIR / filename
    if not file_path.exists():
        raise HTTPException(status_code=404, detail="File not found")
    return FileResponse(file_path)


# Launch example:
# uvicorn app:app --host 0.0.0.0 --port 8000
