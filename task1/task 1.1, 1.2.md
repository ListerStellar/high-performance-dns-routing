**1.1**
- Klic je definovan pomoci direktivy proxy_cache_key. By default: proxy_cache_key $scheme$proxy_host$request_uri;
- Klic se pouziva pro vypocet jmena souboru (po aplikovani cachovaci funkce MD5).
- Umisteni cache je zadano pomoci direktivy proxy_cache_path. 
Napr.: proxy_cache_path /data/nginx/cache levels=1:2 keys_zone=one:10m; znamena, ze soubory budou ulozeny
jako /data/nginx/cache/(prvni znak klice (po MD5) od konce)/(dalsi dva znak klice (po MD5) od konce)/(cely klic po MD5).
Pocet znaku vyse je definovan parametrem *levels*

**1.2**
- Pro pridani popsane funkcionality bych napsal svuj dynamicky C modul (jelikoz nelze vyuizit LUA nebo OpenResty moduly).
Musel bych zasahnout do faze odpovedi na http request a tam pridat hlavicku X-Cache-Key, obsahem ktere bude klic, ktery bych mel vytahnout ze struktury ngx_http_cache_t (r->cache->keys). 
Popravdě řečeno, s psaním vlastních Nginx C modulů zatím mnoho praktických zkušeností nemám, takže pro skutečnou implementaci a přesnou syntaxi bych musel nahlédnout hlouběji do dokumentace Nginx API.