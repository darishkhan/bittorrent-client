# bittorrent-client

An attempt to mimic bittorrent client in C++. 
- Encoding of data structures to Bencoded strings and vice versa.
- A simplified Choke algorithm is applied for downloading pieces.
- Uses sha1 hash to check authenticity of each piece and the torrent file.
- TCP connection with peers.

Note: Does not yet support magnet links. Will add the logic soon.

Further Todos:
- Add multithreading so that multiple pieces can be downloaded simultaneously.
- Support magnet links.
