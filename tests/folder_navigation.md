# HLS folder navigation regression

Use a dedicated local folder containing these three files in name order:

1. `01-before.mp4`: ordinary video.
2. `02-stream.m3u8`: HLS media manifest with `#EXT-X-TARGETDURATION`,
   `#EXTINF` and `#EXT-X-ENDLIST`, referencing MPEG-TS segments in a subfolder.
3. `03-after.mp4`: ordinary video.

Keep the TS segments in a subfolder so they are not separate candidates in the
parent folder. Enable searching in the folder for Next/Previous, use filename
sorting, and use videos without chapters. Start by opening `01-before.mp4`.

Expected toolbar Next sequence: `02-stream.m3u8`, then `03-after.mp4`.
Expected toolbar Previous sequence: `02-stream.m3u8`, then `01-before.mp4`.
The Next/PageDown and Previous/PageUp keyboard shortcuts use the same handlers.
The stream must expose the combined duration, play across TS boundaries, and
remain one item for Next/Previous navigation.

Before the fix, Next skips directly from `01-before.mp4` to `03-after.mp4`.
This failure was reproduced on the initial custom executable. With revision 2,
both directions passed using a 30-second H.264 HLS fixture containing fifteen
2-second TS segments. Playback continued across segment boundaries to 30 seconds.

The exception is enabled only by SearchInDir. Other WildcardFileSearch callers
(including M3U wildcard expansion) retain playlist exclusion, so a wildcard in
a playlist cannot include that same playlist because of this change.
